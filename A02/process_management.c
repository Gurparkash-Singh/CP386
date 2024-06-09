#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

void writeOutput(char *command, char *output) {
    FILE *fp;
    fp = fopen("output.txt", "a");
    fprintf(fp, "The output of: %s : is\n", command);
    fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(1);
    }

    char *inputFileName = argv[1];
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        perror("Failed to open input file");
        exit(1);
    }

    // Create a temporary file to hold shared memory
    int sharedMemoryFD = shm_open("./share", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (sharedMemoryFD == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    // Set the size of the shared memory segment
    if (ftruncate(sharedMemoryFD, 4096) == -1) {
        perror("Failed to set the size of shared memory");
        exit(1);
    }

    // Map the shared memory segment into the address space
    char *sharedMemory = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFD, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process reads the input file
        char buffer[4096];
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), inputFile);
        strncpy(sharedMemory, buffer, bytesRead);
        fclose(inputFile);
        exit(0);
    }

    // Parent process continues from here

    int status;
    waitpid(pid, &status, 0);

    // Execute shell commands
    char *commands = sharedMemory;
    char *command = strtok(commands, "\n");

    while (command != NULL) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Pipe creation failed");
            exit(1);
        }

        pid_t command_pid = fork();
        if (command_pid == -1) {
            perror("Failed to fork a command process");
            exit(1);
        }

        if (command_pid == 0) {
            // Child process executes the command
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            execl("/bin/sh", "/bin/sh", "-c", command, NULL);
            perror("Failed to execute the command");
            exit(1);
        } else {
            close(pipefd[1]);
            char output[4096];
            ssize_t bytesRead = read(pipefd[0], output, sizeof(output));
            close(pipefd[0]);
            output[bytesRead] = '\0';

            // Write the command output to "output.txt"
            writeOutput(command, output);

            waitpid(command_pid, &status, 0);
        }

        command = strtok(NULL, "\n");
    }

    // Clean up shared memory
    munmap(sharedMemory, 4096);
    close(sharedMemoryFD);
    shm_unlink("./share");

    return 0;
}
