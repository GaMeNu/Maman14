#include <stdio.h>
#include "whitespaces.h"

void removeWhitespaces(char *string) {
    /*
     * This function clears up a string from whitespaces to prevent weird comparison issues with macro names
     */
    char *readPtr = string;
    char *writePtr = string;
    if (readPtr == NULL) return;
    while (*readPtr) {
        if (*readPtr != ' ' && *readPtr != '\t' && *readPtr != '\n' && *readPtr != '\r') {
            *writePtr = *readPtr;
            writePtr++;
        }
        readPtr++;
    }

    *writePtr = '\0';

}

void removeLeadingWhitespaces(char *string) {
    /*
     * This function clears leading whitespaces while storing macros to prevent indented pasting of the macros.
     */
    char *readPtr = string;
    char *writePtr = string;
    if (!string)
        return;

    while (*readPtr == ' ' || *readPtr == '\t' || *readPtr == '\n' || *readPtr == '\r'){
        readPtr++;
    }

    while (*readPtr != '\0'){
        *writePtr = *readPtr;
        writePtr++;
        readPtr++;
    }
    *writePtr = '\0';
}
