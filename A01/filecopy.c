#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    /*
        Opens input and output files and writes input file to output file.
        If either file cannot be opened stops the program.
    */
    if (3 > argc)
    {
        printf("Insufficient parameters passed.\n");
        return -1;
    }

    FILE *input_file;
    FILE *output_file;

    input_file = fopen(argv[1], "r");

    if (NULL == input_file)
    {
        printf("Encountered a problem opening input file.\n");
        printf("Exiting....\n");
        return -1;
    }

    output_file = fopen(argv[2], "w");

    if (NULL == output_file)
    {
        printf("Encountered a problem opening output file.\n");
        printf("Exiting....\n");
        return -1;
    }

    char line[100];

    while (NULL != fgets(line, sizeof(line), input_file))
    {
        fprintf(output_file, "%s", line);
    }

    fclose(output_file);
    fclose(input_file);
    return 0;
}
