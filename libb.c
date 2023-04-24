#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    void *lib_handle;
    void (*my_function)(void);
    char *error;

    // Compile the C source file for the dynamic library into a shared object
    system("gcc -shared -fPIC -o mylib.so mylib.c");

    // Load the dynamic library at runtime
    lib_handle = dlopen("./mylib.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    // Obtain a function pointer to the exported function in the library
    my_function = dlsym(lib_handle, "my_function");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    // Call the function from the dynamic library
    (*my_function)();

    // Unload the dynamic library
    dlclose(lib_handle);

    return 0;
}