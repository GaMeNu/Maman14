#include <stdio.h>
#include "errors.h"


struct errorName{
    int value;
    char *name;
};

struct errorName errors[] = {
        {FILE_ERROR,                    "FILE_ERROR"},
        {MEM_ERROR,                     "MEM_ERROR"},
        {MISSING_SYMBOL_INTERNAL_ERROR, "MISSING_SYMBOL_INTERNAL_ERROR"},
        {INVALID_OPERAND_ADDRESSING_INTERNAL_ERROR, "INVALID_OPERAND_ADDRESSING_INTERNAL_ERROR"},
        {SYNTAX_ERROR,                "SYNTAX_ERROR"},
        {UNDECLARED_MACRO_ERROR,      "UNDECLARED_MACRO_ERROR"},
        {DOUBLE_MACRO_ERROR,          "DOUBLE_MACRO_ERROR"},
        {REDEFINED_MACRO_ERROR,       "REDEFINED_MACRO_ERROR"},
        {UNENDED_MACRO_ERROR,         "UNENDED_MACRO_ERROR"},
        {RESERVED_NAME_ERROR,         "RESERVED_NAME_ERROR"},
        {REDEFINED_SYMBOL_ERROR,      "REDEFINED_SYMBOL_ERROR"},
        {UNDEFINED_SYMBOL_ERROR,      "UNDEFINED_SYMBOL_ERROR"},
        {INVALID_OPERAND_ERROR,       "INVALID_OPERAND_ERROR"},
        {INSTRUCTION_NOT_FOUND_ERROR, "INSTRUCTION_NOT_FOUND_ERROR haha 404 get it? Eh it's a dumb joke anyway"},
        {INVALID_INSTRUCTION_ERROR,   "INVALID_INSTRUCTION_ERROR"},
        {OPERAND_COUNT_ERROR,         "OPERAND_COUNT_ERROR"},
        {INVALID_OPERAND_TYPE_ERROR,  "INVALID_OPERAND_TYPE_ERROR"},
        {0, NULL}
};

char *errorCodeToName(int code) {
    struct errorName *currentError;
    for(currentError = errors; currentError->value != 0; currentError++){
        if(currentError->value == code) return currentError->name;
    }
    return NULL;
}

void throwError(int code, int line, char *filename, char *message) {
    if(errorCodeToName(code)) printf("ERROR %d: %s\n", code, errorCodeToName(code));
    else printf("ERROR %d\n", code);
    printf("In file %s at line %d\n", filename, line);
    printf("%s\n\n", message);
}


