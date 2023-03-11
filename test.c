#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000

int main() {
    FILE *input_file, *output_file;
    char input_filename[] = "input.txt";
    char output_filename[] = "output.mc";
    char buffer[MAX_LEN];
    char *token;
    int count = 0;

    // open input file in read mode
    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    // open output file in write mode
    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Error opening output file\n");
        return 1;
    }

    // read contents of input file and remove alternate words
    while (fgets(buffer, MAX_LEN, input_file)) {
        token = strtok(buffer, " ");
        while (token != NULL) {
            count++;
            if (count % 2 == 0) {
                fprintf(output_file, "%s ", token);
            }
            token = strtok(NULL, " ");
        }
    }

    // close files
    fclose(input_file);
    fclose(output_file);

    printf("Alternate words removed\n");

    return 0;
}
