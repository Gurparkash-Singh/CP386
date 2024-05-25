#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef _WIN32
    #include <direct.h>
#endif

void get_dir_name(char * dir_name)
{
    /*
        Auxilliary function to get the name of a directory.
        Used for creating and removing a directory.
    */
    printf("Enter directory name: ");
    if( NULL == fgets(dir_name, sizeof(dir_name), stdin))
    {
        printf("Error reading directory name");
    }
    else
    {
        // Remove any trailing \n characters
        for (int i = 0; i < sizeof(dir_name); i++)
        {
            if ('\0' == dir_name[i] && 1 < i)
            {
                if ('\n' == dir_name[i-1])
                {
                    dir_name[i - 1] = '\0';
                }
            }
        }
    }
    fflush(stdin);
}

void make_dir()
{
    /*
        Creates a directory on Windows, Unix/Linux systems.
    */
    char dir_name[100];
    get_dir_name(dir_name);

    int result;

    #ifdef _WIN32
        result = _mkdir(dir_name);
    #else
        result = mkdir(dir_name, S_IRWXU);
    #endif

    if (-1 == result)
    {
        if (EEXIST == errno)
        {
            printf("Directory %s exists", dir_name);
        }
        else
        {
            perror("Error");
        }
    }
    else
    {
        printf("Directory %s created\n", dir_name);
    }
}

void remove_dir()
{
    /*
        Removes an empty directory on Windows, Unix/Linux Systems.
    */
    char dir_name[100];
    get_dir_name(dir_name);
    
    int result;

    #ifdef _WIN32
        result = _rmdir(dir_name);
    #else
        result = rmdir(dir_name);
    #endif

    if (-1 == result)
    {
        perror("Error");
    }
    else
    {
        printf("Directory %s removed", dir_name);
    }
}

void print_current_dir()
{
    /*
        Switches to parent directory
    */
    char current_dir[100];

    getcwd(current_dir, sizeof(current_dir));
    printf("The current directory is: %s\n", current_dir);    
}

void move_to_parent_dir()
{
    /*
        Moves to parent directory.
    */
    chdir("..");
    printf("Directory Changed\n");
    print_current_dir();
}

void list_dir_structure()
{
    /*
        Lists all files and directories in the current directory.
    */
    DIR *d;
    struct dirent *dir;

    d = opendir(".");

    if (d != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}

void close_dir()
{
    /*
        Exits the program
    */
    exit(0);
}

char menu()
{
    /*
        This function displays a menu.
        It returns a character representing the operation to be performed.
    */
    char op_code;

    printf("Select the option(s) appropriately by entering the number:\n");
    printf("1. Create a directory\n");
    printf("2. Remove a directory\n");
    printf("3. Print current working directory\n");
    printf("4. Change directory one level up\n");
    printf("5. Read the contents of the directory\n");
    printf("6. Close the current directory\n");
    printf("q. Exit the program\n");
    scanf("%c", &op_code);
    fflush(stdin);

    return op_code;
}

int main()
{
    /*
        Handles the main logic of the program.
        Used to display menu and perform the appropriate operation.
    */
    
    char op_code;

    do
    {
        op_code = menu();

        switch(op_code)
        {
            case '1':
                make_dir();
                break;
            case '2':
                remove_dir();
                break;
            case '3':
                print_current_dir();
                break;
            case '4':
                move_to_parent_dir();
                break;
            case '5':
                list_dir_structure();
                break;
            case '6':
                close_dir();
                break;
            // Make sure that nothing happens when q or Q are entered.
            case 'q':
            case 'Q':
                break;
            default:
                printf("Invalid Input! Please try again.\n");
        }
        printf("\n");
    } while ('q' != op_code && 'Q' != op_code);

    // This portion of the code should always exit gracefully.
    return 0;
}
