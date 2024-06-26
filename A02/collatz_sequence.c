#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAXIMUM_NUMBERS 100
#define MAXIMUM_SEQUENCE_LENGTH 100

int main() {
    int start_numbers[MAXIMUM_NUMBERS];
    int num_count = 0;

    FILE* file = fopen("start_numbers.txt", "r");
    if (file == NULL) {
        perror("Error opening the file");
        exit(1);
    }
 
    // Read positive integers from the file and store them in an array
    while (fscanf(file, "%d", &start_numbers[num_count]) == 1) {
        num_count++;
    }

    // close file
    fclose(file);

    // loop through each positive integer from the file
    for (int i = 0; i < num_count; i++) {
        int number = start_numbers[i];

        // create a shared memory object for the Collatz sequence
        int sequence[MAXIMUM_SEQUENCE_LENGTH];
        int shm_fd = shm_open("/collatz_sequence", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, MAXIMUM_SEQUENCE_LENGTH * sizeof(int));
       
        // Map the shared memory into the process's address space
        int* shared_sequence = mmap(0, MAXIMUM_SEQUENCE_LENGTH * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        if (fork() == 0) {
            int seq_count = 0;
            // Child process
           
            while (number != 1 && seq_count < MAXIMUM_SEQUENCE_LENGTH) {
                sequence[seq_count] = number;
                seq_count++;

                if (number % 2 == 0) {
                    number /= 2;
                } else {
                    number = 3 * number + 1;
                }
            }

            sequence[seq_count] = 1;
            seq_count++;

            // Copy the generated sequence to shared memory
            for (int j = 0; j < seq_count; j++) {
                shared_sequence[j] = sequence[j];
            }

            // print the Collatz sequence generated by the child process
            printf("Child Process: The generated collatz sequence is ");
            for (int j = 0; j < seq_count; j++) {
                printf("%d ", shared_sequence[j]);
            }
            printf("\n");

            // cleanup and exit the child process
            munmap(shared_sequence, MAXIMUM_SEQUENCE_LENGTH * sizeof(int));
            close(shm_fd);
            exit(0);
        } else {
            // parent function
            printf("Parent Process: The positive integer read from the file is %d\n", start_numbers[i]);
            wait(NULL);
        }
    }

    return 0;
}
