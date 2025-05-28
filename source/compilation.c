#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "errors.h"
#include "statements.h"
#include "compilation.h"

/* Generated externally using a python script */
struct {
    unsigned char val:6;
    char ch;
} base64Table[] = {
        {0, 'A'},
        {1, 'B'},
        {2, 'C'},
        {3, 'D'},
        {4, 'E'},
        {5, 'F'},
        {6, 'G'},
        {7, 'H'},
        {8, 'I'},
        {9, 'J'},
        {10, 'K'},
        {11, 'L'},
        {12, 'M'},
        {13, 'N'},
        {14, 'O'},
        {15, 'P'},
        {16, 'Q'},
        {17, 'R'},
        {18, 'S'},
        {19, 'T'},
        {20, 'U'},
        {21, 'V'},
        {22, 'W'},
        {23, 'X'},
        {24, 'Y'},
        {25, 'Z'},
        {26, 'a'},
        {27, 'b'},
        {28, 'c'},
        {29, 'd'},
        {30, 'e'},
        {31, 'f'},
        {32, 'g'},
        {33, 'h'},
        {34, 'i'},
        {35, 'j'},
        {36, 'k'},
        {37, 'l'},
        {38, 'm'},
        {39, 'n'},
        {40, 'o'},
        {41, 'p'},
        {42, 'q'},
        {43, 'r'},
        {44, 's'},
        {45, 't'},
        {46, 'u'},
        {47, 'v'},
        {48, 'w'},
        {49, 'x'},
        {50, 'y'},
        {51, 'z'},
        {52, '0'},
        {53, '1'},
        {54, '2'},
        {55, '3'},
        {56, '4'},
        {57, '5'},
        {58, '6'},
        {59, '7'},
        {60, '8'},
        {61, '9'},
        {62, '+'},
        {63, '/'},
        {0, 0}
};

int compile(char *filename, int ic, int dc, dataListNode *instructionTable, dataListNode *dataTable, NAListNode *entryTable, NAListNode *externTable) {
    char outputFName[FILENAME_MAX];
    FILE *outputFile;

    strcpy(outputFName, filename);
    strcat(outputFName, ".ob");

    outputFile = fopen(outputFName, "w");
    if (!outputFile){
        throwError(FILE_ERROR, -1, outputFName, "Failed to open/create file.");
        return 1;
    }

    fprintf(outputFile, "%d %d\n", ic, dc);

    writeListToFile(instructionTable, outputFile);
    writeListToFile(dataTable, outputFile);

    writeSymbolsFiles(filename, entryTable, externTable);

    return 0;
}

void writeListToFile(dataListNode *dataTable, FILE *outputFile) {
    char opcode;
    char nibble;
    dataListNode *curWord;
    char word[3];
    word[2] = '\0';

    curWord = dataTable;
    while (curWord){
        if (curWord->dataType == INST_WORD){
            /* Assemble into binary for INSTRUCTION */

            /*
             * assemble nibble one
             * (nibble == half a word's bits)
             */
            nibble = (char) (curWord->instWord->srcOp);
            nibble = (char) (nibble << 3);
            opcode = (char) (curWord->instWord->opCode >> 1);
            nibble = (char) (nibble | opcode);
            word[0] = getMatchingChar(nibble);

            /*printBinaryNumber(nibble);
            printf("\n");*/

            /* Assemble nibble 2 */
            nibble = (char) ((curWord->instWord->opCode & 1) << 3);
            nibble = (char) (nibble | curWord->instWord->destOp);
            nibble = (char) (nibble << 2);
            nibble = (char) (nibble | curWord->instWord->encoding);
            word[1] = getMatchingChar(nibble);

            /*printBinaryNumber(nibble);
            printf("\n");*/

            /* write both nibbles to file */
            fprintf(outputFile, "%s\n", word);


        } else if(curWord->dataType == DATA_WORD) {
            /* Assemble into binary for DATA*/
            nibble = (char) (curWord->dataWord->data >> 6);
            word[0] = getMatchingChar(nibble);
            /*printBinaryNumber(nibble);
            printf(" ");*/

            nibble = (char) (curWord->dataWord->data & 63);
            word[1] = getMatchingChar(nibble);
            /*
            printBinaryNumber(nibble);
            printf("\n");

            Oh wow a debug message, I didn't know that programmers aren't perfect
            and need to debug their own programs
            */

            fprintf(outputFile, "%s\n", word);
        }
        curWord = curWord->next;

    }
}

int writeSymbolsFiles(char *filename, NAListNode *entryTable, NAListNode *externalsTable) {
    NAListNode* curSymbol;

    FILE *extFile = NULL;
    FILE *entFile = NULL;

    char entFilename[FILENAME_MAX];
    char extFilename[FILENAME_MAX];

    strcpy(entFilename, filename);
    strcpy(extFilename, filename);

    strcat(entFilename, ".ent");
    strcat(extFilename, ".ext");

    curSymbol = entryTable;
    /* repeat for entries and write them */
    while (curSymbol){
        createNewFile(&entFile, entFilename);
        if (!entFile){
            throwError(FILE_ERROR, 0, entFilename, "Failed to create entry file.");
            return 1;
        }

        fprintf(entFile, "%s %d\n", curSymbol->entry->name, curSymbol->entry->address);

        curSymbol = curSymbol->next;
    }

    fclose(entFile);

    curSymbol = externalsTable;
    /* repeat for entries and write them */
    while(curSymbol){
        createNewFile(&extFile, extFilename);
        if (!extFile){
            throwError(FILE_ERROR, 0, extFilename, "Failed to create externals file.");
            return 1;
        }

        fprintf(extFile, "%s %d\n", curSymbol->entry->name, curSymbol->entry->address);
        curSymbol = curSymbol->next;
    }

    fclose(extFile);

    return 0;
}

void createNewFile(FILE **file, char *filename) {
    if (!(*file)){
        /* Create entry file */
        *file = fopen(filename, "w");
    }
}


char getMatchingChar(char val) {
    char i = 0;
    while (base64Table[i].ch != 0){

        if (base64Table[i].val == val) return base64Table[i].ch;
        i++;
    }
    return 0;
}
