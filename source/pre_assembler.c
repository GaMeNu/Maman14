#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "whitespaces.h"
#include "statements.h"
#include "pre_assembler.h"

int preAssemble(char *filename, int *lineCounter){
    FILE *input;
    FILE *output;
    macroList *macroTable;
    macro *currentMacro;
    macro *replaceMacro;
    char currentLine[LINE_SIZE];
    char currentLineCpy[LINE_SIZE];
    char mcroName[MAX_WORD_SIZE];
    char *firstToken;
    char *contentBuffer = NULL;
    char *reallocBuffer;
    int bufferSize;
    char inputFName[LINE_SIZE], outputFName[LINE_SIZE];
    int readingMacro = 0;
    *lineCounter = 0;
    int errCount = 0;
    char errMsgBuffer[ERROR_MESSAGE_BUFFER_SIZE];
    char filenameBuffer[FILENAME_MAX];

    strcpy(filenameBuffer, filename);
    strcat(filenameBuffer, ".as");

    strcpy(inputFName,filename);
    strcpy(outputFName, filename);

    strcat(inputFName, ".as");
    strcat(outputFName, ".am");

    input = fopen(inputFName, "r");
    if (!input){
        throwError(FILE_ERROR, 0, inputFName, "Failed to open input file, does the file exist in the program's directory?");
        return 1;
    }

    output = fopen(outputFName, "w");
    if (!output){
        fclose(input);
        throwError(FILE_ERROR, 0, inputFName, "Failed to open output file.");
        return 1;
    }

    macroTable = NULL;

    /*
     * repeat for all lines
     */
    while (fgets(currentLine, LINE_SIZE, input)) {
        (*lineCounter)++;
        removeLeadingWhitespaces(currentLine);

        /* Empty or comment */
        if (currentLine[0] == ';' || currentLine[0] == '\n'){
            continue;
        }
        /* currentLineCpy to not damage currentLine with strtok */
        strcpy(currentLineCpy, currentLine);
        firstToken = strtok(currentLineCpy, " ");
        if (firstToken == NULL) continue;
        removeWhitespaces(firstToken);
        if(firstToken[0] == ';') continue;

        /* if the first token was "mcro" - mcro definition */
        if (strcmp(firstToken, "mcro") == 0){
            /* are we already reading a macro? (Macro defined within macro) */
            if (readingMacro == 1){
                strcpy(errMsgBuffer, "Defined macro, but already reading macro \"");
                strcat(errMsgBuffer, mcroName);
                strcat(errMsgBuffer, "\"");

                throwError(DOUBLE_MACRO_ERROR, *lineCounter, filenameBuffer, errMsgBuffer);
                errCount++;

            }
            strcpy(mcroName, strtok(NULL, " "));
            removeWhitespaces(mcroName);

            if (getMacro(macroTable, mcroName)){

                strcpy(errMsgBuffer, "Redefined macro with name \"");
                strcat(errMsgBuffer, mcroName);
                strcat(errMsgBuffer, "\"");

                throwError(REDEFINED_MACRO_ERROR, *lineCounter, filenameBuffer, errMsgBuffer);
                errCount++;
            }
            if (isReserved(mcroName) == 1){
                strcpy(errMsgBuffer, "Macro defined uses reserved language keyword \"");
                strcat(errMsgBuffer, mcroName);
                strcat(errMsgBuffer, "\"");

                throwError(RESERVED_NAME_ERROR, *lineCounter, filenameBuffer, errMsgBuffer);
                errCount++;
            }

            readingMacro = 1;
            contentBuffer = (char *) malloc(LINE_SIZE*sizeof(char));
            if (contentBuffer == NULL){
                fclose(input);
                fclose(output);
                freeMacroList(macroTable);
                throwError(MEM_ERROR, *lineCounter, filenameBuffer, "Memory allocation failed. Fatal error.");
                return (errCount+1);
            }
            *contentBuffer = '\0';
            bufferSize = LINE_SIZE;
        }

        else if (strcmp(firstToken, "endmcro") == 0){
            if(readingMacro == 0){
                throwError(UNDECLARED_MACRO_ERROR, *lineCounter, filenameBuffer, "Macro definition ended but macro was never declared.");
                errCount++;
            }
            currentMacro = newMacro(mcroName, contentBuffer);
            if (!currentMacro){
                fclose(input);
                fclose(output);
                freeMacroList(macroTable);
                throwError(MEM_ERROR, *lineCounter, filenameBuffer, "Memory allocation failed. Fatal error.");
                return (errCount+1);
            }

            appendMacro(currentMacro, &macroTable);

            if (contentBuffer != NULL){
                free(contentBuffer);
            }

            readingMacro = 0;
        }

        else if (readingMacro == 0) {
            replaceMacro = getMacro(macroTable, firstToken);
            if (replaceMacro == NULL)
                fputs(currentLine, output);
            else{
                fputs(replaceMacro->value, output);
            }
        }
        else {
            if (strlen(contentBuffer) + strlen(currentLine) > bufferSize){
                /*mem realloced here*/
                bufferSize += LINE_SIZE;
                reallocBuffer = (char *) realloc(contentBuffer, bufferSize*sizeof(char));
                if (reallocBuffer == NULL){
                    free(contentBuffer);
                    fclose(input);
                    fclose(output);
                    freeMacroList(macroTable);
                    throwError(MEM_ERROR, *lineCounter, filenameBuffer, "Memory allocation failed. Fatal error.");
                    return (errCount+1);
                }
                contentBuffer=reallocBuffer;
                /*end mem realloc*/
            }
            /*add new line to buffer*/
            removeLeadingWhitespaces(currentLine);
            strcat(contentBuffer, currentLine);
        }


    }

    if (readingMacro == 1){
        strcpy(errMsgBuffer, "Reached End-of-File while reading macro \"");
        strcat(errMsgBuffer, mcroName);
        strcat(errMsgBuffer, "\"");

        throwError(UNENDED_MACRO_ERROR, *lineCounter, filenameBuffer, errMsgBuffer);
        errCount++;
    }

    /* Final tasks: free the macro table, and close input and output files */
    freeMacroList(macroTable);

    fclose(input);
    fclose(output);

    if (errCount != 0){
        strcpy(filenameBuffer, filename);
        strcat(filenameBuffer, ".am");
        remove(filenameBuffer);
    }

    return errCount;

}

macro *newMacro(char *name, char *content) {
    /*
     * This function creates a new macro definition.
     */
    macro *retMacro;
    int namelen;
    int contentlen;
    retMacro = (macro *) malloc(sizeof(macro));
    if (retMacro == NULL){
        return NULL;
    }

    namelen = (int) strlen(name);
    retMacro->name = (char *) malloc(namelen*sizeof(char));
    if (retMacro->name == NULL){
        free(retMacro);
        return NULL;
    }
    strcpy(retMacro->name, name);

    contentlen = (int) strlen(content) + 1;

    retMacro->value = (char *) malloc(contentlen*sizeof(char));
    if (retMacro->value == NULL){
        free(retMacro->name);
        free(retMacro);
        return NULL;
    }
    strcpy(retMacro->value, content);

    return retMacro;
}

void printTable(macroList *macrolist){
    macroList *currentEntry;
    macro *curMacro;
    for (currentEntry=macrolist; currentEntry != NULL; currentEntry = currentEntry->next){
        curMacro = currentEntry->macro;
        printf("%s\n", curMacro->name);
        printf("%s\n", curMacro->value);
        printf("\n");
    }
}

void freeMacroList(macroList *list) {
    macroList *current = list;
    macroList *next = NULL;

    while (current != NULL) {
        next = current->next;

        /* Free the macro */
        if(current->macro != NULL) {
            freeMacro(current->macro);
        }

        /* Free the macroList node */
        free(current);

        current = next;
    }
}

void appendMacro(macro *mcro, macroList **table) {
    /*
     * We add the macro to a macro table, just go through the list and put it on the first place.
     */
    macroList *newNode = (macroList *) malloc(sizeof(macroList));
    if (newNode == NULL) return;
    newNode->macro = mcro;
    newNode->next = NULL;

    if (*table == NULL) {
        *table = newNode;
    } else {
        macroList *temp = *table;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

}

void freeMacro(macro *mcro) {
    free(mcro->name);
    free(mcro->value);
    free(mcro);
}

macro *getMacro(macroList* head, char* macroName) {
    /*
     * Just go through the list and compare the names until a match found, otherwise we return NULL
     */
    macroList *current = head;
    if (current == NULL){
        return NULL;
    }

    while (current != NULL) {
        if (strcmp(current->macro->name, macroName) == 0) {
            return current->macro;  /* Macro found */
        }
        current = current->next;
    }
    return NULL;
}


