#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "errors.h"
#include "whitespaces.h"
#include "statements.h"
#include "lists.h"
#include "instructions.h"

#include "pass_one.h"

int pass1(char *filename, int *ic, int *dc, int *linePtr, NAListNode **symbolList, dataListNode **dataList){
    FILE *input;
    char inputFName[LINE_SIZE];
    char currentLine[LINE_SIZE];
    char currentLineCpy[LINE_SIZE];
    char labelName[MAX_WORD_SIZE];
    char *token;
    NAListNode *symbolTable = NULL;
    namedAddress *nNANode;
    dataListNode *dataTable = NULL;
    dataNode *nDataNode;
    char readingSymbol;
    int currentWord;
    char *endptr = NULL;
    char *tokenPtr;
    namedAddress *NAPtr;
    int operandCount;

    int errCount = 0;
    char errMsgBuffer[ERROR_MESSAGE_BUFFER_SIZE];

    int instructionCounter = 0, dataCounter = 0;
    *linePtr = 0;



    strcpy(inputFName,filename);
    strcat(inputFName, ".am");
    input = fopen(inputFName, "r");

    if (!input){
        perror("Failed to open input file");

        throwError(FILE_ERROR, *linePtr, inputFName, errMsgBuffer);
        return 1;
    }


    while (fgets(currentLine, LINE_SIZE, input)) {
        (*linePtr)++;

        readingSymbol = 0;

        strcpy(currentLineCpy, currentLine);
        token = strtok(currentLineCpy, " ");
        removeWhitespaces(token);

        /* check if token is a label */
        if(isSymbol(token)){
            strcpy(labelName, token);
            labelName[strlen(labelName)-1] = '\0';
            if(getNamedAddress(symbolTable, labelName) != NULL){
                strcpy(errMsgBuffer, "Label \"");
                strcat(errMsgBuffer, labelName);
                strcat(errMsgBuffer, "\" was already defined");

                throwError(REDEFINED_SYMBOL_ERROR, *linePtr, inputFName, errMsgBuffer);
                errCount++;
            }

            readingSymbol = 1;

        }

        /* Move on to deal with the next command; */
        if (readingSymbol == 1){
            token = strtok(NULL, " ");
        }
        removeWhitespaces(token);

        /* deal with directives */
        if(strcmp(token, ".string") == 0){
            if(readingSymbol == 1) {
                nNANode = newEntry(labelName, dataCounter, DATA);
                if (!nNANode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendNAListNode(&symbolTable, nNANode);
            }
            token = strtok(NULL, "\"");

            tokenPtr = token;
            /*
             * repeat for each char in string
             * Normal quote character: ""
             * OpenU task book quote characters: “”
             * These are outside of ASCII. They are UTF-8. Why not just use NORMAL QUOTES???
             * It took me days to figure this out >:(
             */
            while (*tokenPtr != '\0'){

                if(*tokenPtr == '\n'){
                    throwError(INVALID_OPERAND_ERROR, *linePtr, inputFName, "Unterminated string.");
                    errCount++;
                }
                nDataNode = newDataNode(*tokenPtr);
                if (!nDataNode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendDataNode(&dataTable, nDataNode);
                dataCounter++;
                tokenPtr++;
            }

            nDataNode = newDataNode(0);
            if (!nDataNode) {
                freeLists(symbolTable, dataTable);
                throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                return (errCount+1);
            }
            appendDataNode(&dataTable, nDataNode);
            dataCounter++;

        } else if(strcmp(token, ".data") == 0){
            if(readingSymbol == 1) {
                nNANode = newEntry(labelName, dataCounter, DATA);
                if (!nNANode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendNAListNode(&symbolTable, nNANode);
            }
            /* Keep reading these */
            token = strtok(NULL, ",");
            while (token){
                removeWhitespaces(token);

                currentWord = strtol(token, &endptr, 10);
                if (errno != 0 || *endptr){
                    strcpy(errMsgBuffer, "Operand \"");
                    strcat(errMsgBuffer, token);
                    strcat(errMsgBuffer, "\" is an invalid number.");

                    throwError(INVALID_OPERAND_ERROR, *linePtr, inputFName, errMsgBuffer);
                    errCount++;
                }
                nDataNode = newDataNode(currentWord);
                if (!nDataNode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendDataNode(&dataTable, nDataNode);
                dataCounter++;
                token = strtok(NULL, ",");
            }

        } else if(strcmp(token, ".extern") == 0) {
            /* Handle .extern (similar to .data) */
            token = strtok(NULL, ",");
            while(token != NULL){
                removeWhitespaces(token);
                if (getNamedAddress(symbolTable, token) != NULL) {
                    strcpy(errMsgBuffer, "Label \"");
                    strcat(errMsgBuffer, token);
                    strcat(errMsgBuffer, "\" was already defined");

                    throwError(REDEFINED_SYMBOL_ERROR, *linePtr, inputFName, errMsgBuffer);
                    errCount++;
                }

                nNANode = newEntry(token, 0, EXTERNAL);
                if (!nNANode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendNAListNode(&symbolTable, nNANode);

                token = strtok(NULL, ",");
            }
        } else if(strcmp(token, ".entry") == 0){
            /* Handle .entry */
            token = strtok(NULL, ",");
            removeWhitespaces(token);
            NAPtr = getNamedAddress(symbolTable, token);

            if (NAPtr != NULL){
                if(NAPtr->type == ENTRY || NAPtr->type == EXTERNAL){
                    strcpy(errMsgBuffer, "Label \"");
                    strcat(errMsgBuffer, token);
                    strcat(errMsgBuffer, "\" was already defined");

                    throwError(REDEFINED_SYMBOL_ERROR, *linePtr, inputFName, errMsgBuffer);
                    errCount++;
                }
            }
        } else if(isInstruction(token)) {
            if(readingSymbol == 1) {
                nNANode = newEntry(labelName, instructionCounter, CODE);
                if (!nNANode){
                    throwError(MEM_ERROR, *linePtr, inputFName, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                appendNAListNode(&symbolTable, nNANode);
            }

            operandCount = instructionOpCount(token);
            instructionCounter += operandCount + 1;

            /* Check if both operands are registers, and thus share one word */
            token = strtok(NULL, ",");
            if (operandCount == 2){
                removeWhitespaces(token);
                if (token[0] == '@') {
                    token = strtok(NULL, ",");
                    removeWhitespaces(token);
                    if (token[0] == '@') {
                        instructionCounter -= 1;
                    }
                }
            }

        } else {
            strcpy(errMsgBuffer, "Instruction \"");
            strcat(errMsgBuffer, token);
            strcat(errMsgBuffer, "\" is invalid.");

            throwError(INVALID_INSTRUCTION_ERROR, *linePtr, inputFName, errMsgBuffer);
            errCount++;
        }

    }

    /* finishing tasks */
    updateAddresses(symbolTable, instructionCounter, DATA);
    updateAddresses(symbolTable, 100, DATA);
    updateAddresses(symbolTable, 100, CODE);

    *ic = instructionCounter;
    *dc = dataCounter;

    *symbolList = symbolTable;
    *dataList = dataTable;

    fclose(input);
    return 0;
}


