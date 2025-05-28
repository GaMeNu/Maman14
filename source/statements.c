
#include <string.h>
#include "statements.h"
#define ADDRESSINGS_COUNT 3

char *directives[] = {".data",".string",".entry",".extern"};

struct instruction{
    char *name;
    char operandCount;
} instructions [] = {
        {"mov",2},
        {"cmp",2},
        {"add",2},
        {"sub",2},
        {"not",1},
        {"clr",1},
        {"lea",2},
        {"inc",1},
        {"dec",1},
        {"jmp",1},
        {"bne",1},
        {"red",1},
        {"prn",1},
        {"jsr",1},
        {"rts",0},
        {"stop",0}
};

typedef struct instAddressing{
    char *name;
    int srcAddressings[ADDRESSINGS_COUNT];
    int destAddressings[ADDRESSINGS_COUNT];
} instAddressing;
instAddressing instAddressings[] = {
        {"mov",{IMMEDIATE,DIRECT,DIRECT_REGISTER}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"cmp",{IMMEDIATE,DIRECT,DIRECT_REGISTER}, {IMMEDIATE,DIRECT,DIRECT_REGISTER}},
        {"add",{IMMEDIATE,DIRECT,DIRECT_REGISTER}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"sub",{IMMEDIATE,DIRECT,DIRECT_REGISTER}, {NONE, DIRECT,DIRECT_REGISTER}},

        {"not", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"clr", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},

        {"lea", {NONE, DIRECT, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},

        {"inc", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"dec", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"jmp", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"bne", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"red", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},
        {"prn", {NONE, NONE, NONE}, {IMMEDIATE, DIRECT,DIRECT_REGISTER}},
        {"jsr", {NONE, NONE, NONE}, {NONE, DIRECT,DIRECT_REGISTER}},

        {"rts", {NONE, NONE, NONE}, {NONE, NONE, NONE}},
        {"stop", {NONE, NONE, NONE}, {NONE, NONE, NONE}},

};

int isDirective(char *word) {
    int i;
    for (i = 0; i < DIRECTIVE_AMOUNT; i++){
        if(strcmp(word,directives[i]) == 0){
            return 1;
        }
    }
    return 0;
}

int isInstruction(char *word) {
    int i;
    for(i = 0; i < INSTRUCTION_AMOUNT; i++){
        if(strcmp(word, instructions[i].name) == 0){
            return 1;
        }
    }
    return 0;
}

int isReserved(char *word) {
    if (isDirective(word) == 1) return 1;
    return isInstruction(word);
}

int instructionCode(char *word) {
    int i;
    for (i=0; i < INSTRUCTION_AMOUNT; i++){
        if(strcmp(word,instructions[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int instructionOpCount(char *word) {
    int i;
    for(i=0; i< INSTRUCTION_AMOUNT; i++){
        if(strcmp(word,instructions[i].name) == 0) return instructions[i].operandCount;
    }
    return -1;
}

int srcAddressingMatches(char *word, int addressing) {
    instAddressing inst;
    int i = 0;

    if (addressing != IMMEDIATE && addressing != DIRECT && addressing != DIRECT_REGISTER) return 1;

    while (strcmp(word, instAddressings[i].name) != 0){
        i++;
        if (i >= INSTRUCTION_AMOUNT) return 0;
    }
    inst = instAddressings[i];

    switch (addressing) {
        case IMMEDIATE:
            if (inst.srcAddressings[0] == NONE) return 0;
            else return 1;
        case DIRECT:
            if (inst.srcAddressings[1] == NONE) return 0;
            else return 1;
        case DIRECT_REGISTER:
            if (inst.srcAddressings[2] == NONE) return 0;
            else return 1;
        default:
            return 0;
    }
    return 0;
}

int destAddressingMatches(char *word, int addressing) {
    instAddressing inst;
    int i = 0;

    if (addressing != IMMEDIATE && addressing != DIRECT && addressing != DIRECT_REGISTER) return 1;

    while (strcmp(word, instAddressings[i].name) != 0){
        i++;
        if (i >= INSTRUCTION_AMOUNT) return 0;
    }
    inst = instAddressings[i];

    switch (addressing) {
        case IMMEDIATE:
            if (inst.destAddressings[0] == NONE) return 0;
            else return 1;
        case DIRECT:
            if (inst.destAddressings[1] == NONE) return 0;
            else return 1;
        case DIRECT_REGISTER:
            if (inst.destAddressings[2] == NONE) return 0;
            else return 1;
        default:
            return 0;
    }
    return 0;
}
