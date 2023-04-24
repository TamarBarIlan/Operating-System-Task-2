#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This program copies the contents of one file to another.
// The filenames of the input file (file1) and output file (file2)
// are passed as command-line arguments.
// The program also accepts two optional flags: -f (force) and -v (verbose).

int main(int argc, char *argv[]) {

    int force = 0 ; //flag that allows to overwrite the target file
    int verbose = 0; //flag that will output “success” if the file is copied, or “target file exist” if this is
                     //the case, or “general failure” on some other problem
    char *file1, *file2;

    // Check if the minimum required arguments are present
    if (argc < 3) {
        printf("Usage: copy <file1> <file2> [-f] [-v]\n");
        return 1;
    }

    // Parse the arguments
    file1 = argv[1];
    file2 = argv[2];
    //checking flags:
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            force = 1;      // Set force flag if present
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;    // Set verbose flag if present
        }
    }

    // Check if file2 exists and force flag is not set
    if (!force) {
        FILE *fp = fopen(file2, "r");   // Try to open file2 in read mode
        if (fp != NULL) {
            fclose(fp);                 // If it exists, close the file and exit with error
            if (verbose) {
                printf("target file exist\n");
            }
            return 1;
        }
    }

    // Open the input file
    FILE *fin = fopen(file1, "rb");    // Open file1 in binary read mode
    if (fin == NULL) {
        if (verbose) {
            printf("general failure\n");    // If unable to open, exit with error
        }
        return 1;
    }

    // Open the output file
    FILE *fout = fopen(file2, "wb");   // Open file2 in binary write mode
    if (fout == NULL) {
        fclose(fin);
        if (verbose) {
            printf("general failure\n");    // If unable to open, exit with error
        }
        return 1;
    }

    // Copy the contents of the input file to the output file
    char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {    // Read from fin and write to fout until EOF
        fwrite(buffer, 1, n, fout);
    }

    // Close the files
    fclose(fin);
    fclose(fout);

    if (verbose) {
        printf("success\n");    // If verbose flag is set, print success message
    }
    return 0;
}
