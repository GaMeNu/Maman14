#include <stdio.h>
#include <string.h>
#include "consts.h"
#include "errors.h"
#include "lists.h"
#include "whitespaces.h"
#include "pass_two.h"
#include "statements.h"
#include "instructions.h"

int pass2(char *filename, int *linePtr, NAListNode *symbolList, NAListNode **entryList, NAListNode **externList, dataListNode **dataList) {
    FILE *input;
    char inputFName[LINE_SIZE];
    char currentLine[LINE_SIZE];
    char currentLineCpy[LINE_SIZE];
    char labelName[MAX_WORD_SIZE];
    char *token, *curLinePtr, *cpyPtr;
    namedAddress *label;
    namedAddress *entry;
    int errCount = 0;
    char errMsgBuffer[ERROR_MESSAGE_BUFFER_SIZE];

    int instructionCounter = 0;
    *linePtr = 0;

    strcpy(inputFName,filename);
    strcat(inputFName, ".am");
    input = fopen(inputFName, "r");

    if (!input){
        throwError(FILE_ERROR, *linePtr, inputFName, "Failed to open input file.");
        return 1;
    }

    /*
     * For each line in file
     */
    while (fgets(currentLine, LINE_SIZE, input)) {
        (*linePtr)++;


        if (currentLine[0] == ';') continue;

        curLinePtr = currentLine;

        strcpy(currentLineCpy, currentLine);
        cpyPtr = currentLineCpy;

        token = strtok(currentLineCpy, " ");
        cpyPtr += (strlen(cpyPtr) + 1);
        removeWhitespaces(token);

        /* if first token is a symbol - ignore it and get the next one */
        if (isSymbol(token) == 1){
            strtok(currentLine, " ");
            curLinePtr += (strlen(curLinePtr) + 1);

            token = strtok(cpyPtr, " ");
            removeWhitespaces(token);
            cpyPtr += (strlen(cpyPtr) + 1);
        }

        if (isDirective(token)){
            /*
             * Final .entry handling
             * add entry into entry list, or throw an error for undefined label
             */
            if (strcmp(token, ".entry") == 0){
                strcpy(labelName, cpyPtr);
                removeWhitespaces(labelName);
                label = getNamedAddress(symbolList, labelName);
                if (!label){
                    strcpy(errMsgBuffer, "Label \"");
                    strcat(errMsgBuffer, labelName);
                    strcat(errMsgBuffer, "\" is undefined.");

                    throwError(UNDEFINED_SYMBOL_ERROR, *linePtr, inputFName, errMsgBuffer);
                    errCount++;
                    continue;
                }

                entry = newEntry(label->name, label->address, ENTRY);
                if (!entry){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount + 1);
                }

                appendNAListNode(entryList, entry);

            }
            /* can't process this line - it's a directive */
            continue;
        }

        process_instruction(curLinePtr, dataList, symbolList, externList,
                            &instructionCounter, &errCount, inputFName, *linePtr);
    }


    return errCount;
}
