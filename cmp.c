// for run:
//  gcc cmp.c -o cmp
// ./cmp file1.txt file2.txt -v -i

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 1024

/* Function to compare two files and check if they are equal or not */
int compare_files(char* file1, char* file2, int ignore_case, int verbose) {
    FILE* fp1 = fopen(file1, "r");
    FILE* fp2 = fopen(file2, "r");
    if (!fp1 || !fp2) { // Check if files were opened successfully
        printf("Error opening files.\n");
        return -1;
    }

    char buf1[BUF_SIZE];
    char buf2[BUF_SIZE];
    int line_num = 1;
    int is_equal = 1;
    while (fgets(buf1, BUF_SIZE, fp1) != NULL && fgets(buf2, BUF_SIZE, fp2) != NULL) {
        /* If case-insensitive flag is on, convert characters to lowercase */
        if (ignore_case) {
            int len = strlen(buf1);
            for (int i = 0; i < len; i++) {
                buf1[i] = tolower(buf1[i]);
            }
            len = strlen(buf2);
            for (int i = 0; i < len; i++) {
                buf2[i] = tolower(buf2[i]);
            }
        }
        /* Compare two lines and check if they are equal or not */
        if (strcmp(buf1, buf2) != 0) {
            is_equal = 0;
            // if (verbose) {
            //     printf("Line %d is distinct.\n", line_num);
            // }
            break;
        }
        line_num++;
    }

    /* Check if files are equal or not */
    if (is_equal && fgets(buf2, BUF_SIZE, fp2) == NULL) {
        if (verbose) {
            printf("The files are equal.\n");
        }
        return 0;
    } else {
        if (verbose) {
            printf("The files are distinct.\n");
        }
        return 1;
    }

    fclose(fp1); //Close file1
    fclose(fp2); //Close file2
}

int main(int argc, char* argv[]) {
    /* Check if the required arguments have been provided */
    if (argc < 3) { //we must have at least 3 arguments
        printf("Usage: cmp <file1> <file2> [-v] [-i]\n");
        return -1;
    }

    int ignore_case = 0;
    int verbose = 0;

    /* Check if additional flags are provided */
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) { //If -i flag is provided, set ignore_case flag to 1
            ignore_case = 1;
        } else if (strcmp(argv[i], "-v") == 0) { //If -v flag is provided, set verbose flag to 1
            verbose = 1;
        }
    }

    /* Compare two files and get the result */
    int result = compare_files(argv[1], argv[2], ignore_case, verbose);

    return result; // Return the result of comparison
}