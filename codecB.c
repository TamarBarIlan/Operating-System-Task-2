// codecB library implementation

char* codecB_encode(char* message) ;
char* codecB_decode(char* message) ;
#include <string.h>
#include <stdlib.h>

char* codecB_encode(char* message) {
    int len = strlen(message);
    char* encoded = (char*) malloc(len + 1);
    for (int i = 0; i < len; i++) {
        encoded[i] = message[i] + 3;
    }
    encoded[len] = '\0';
    return encoded;
}

char* codecB_decode(char* message) {
    int len = strlen(message);
    char* decoded = (char*) malloc(len + 1);
    for (int i = 0; i < len; i++) {
        decoded[i] = message[i] - 3;
    }
    decoded[len] = '\0';
    return decoded;
}
