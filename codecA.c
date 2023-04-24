char* codecA_encode(char* message) ;
char* codecA_decode(char* message) ;


// codecA library implementation

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
char* codecA_encode(char* message) {
    int len = strlen(message);
    char* encoded = (char*) malloc(len + 1);
    for (int i = 0; i < len; i++) {
        if (islower(message[i])) {
            encoded[i] = toupper(message[i]);
        } else if (isupper(message[i])) {
            encoded[i] = tolower(message[i]);
        } else {
            encoded[i] = message[i];
        }
    }
    encoded[len] = '\0';
    return encoded;
}

char* codecA_decode(char* message) {
    return codecA_encode(message); // codecA is reversible, decoding is equivalent to encoding
}
