// decode tool implementation

#include <stdio.h>
#include "codecA.h"
#include "codecB.h"
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Usage: encode/decode <codec> <message>\n");
        return 1;
    }

    char *codec = argv[1];
    char *message = argv[2];
    char *decoded;

    if (strcmp(codec, "codecA") == 0)
    {
        void *lib_handle = NULL;
        char *(*my_function)(char *) = NULL;
        // char *error = NULL;

        // Load the dynamic library at runtime
        lib_handle = dlopen("./libcodecA.so", RTLD_LAZY);

        if (lib_handle == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        // Obtain a function pointer to the exported function in the library
        my_function = dlsym(lib_handle, "codecA_decode");
        if (my_function == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        // Call the function from the dynamic library
        // (*my_function)(message);
        decoded = my_function(message);
        if (decoded == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        // Unload the dynamic library
        dlclose(lib_handle);
    }
    else if (strcmp(codec, "codecB") == 0)
    {
        void *lib_handle = NULL;
        char *(*my_function)(char *) = NULL;
        // char *error = NULL;

        // Load the dynamic library at runtime
        lib_handle = dlopen("./libcodecB.so", RTLD_LAZY);

        if (lib_handle == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        // Obtain a function pointer to the exported function in the library
        my_function = dlsym(lib_handle, "codecB_decode");
        if (my_function == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        // Call the function from the dynamic library
        decoded = my_function(message);
        if (decoded == NULL)
        {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }
        // Unload the dynamic library
        dlclose(lib_handle);
    }
    else
    {
        printf("Unknown codec: %s\n", codec);
        return 1;
    }

    printf("%s\n", decoded);
    free(decoded);
    return 0;
}
