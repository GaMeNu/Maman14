#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"
#include "errors.h"
#include "lists.h"
#include "pass_one.h"
#include "pass_two.h"
#include "compilation.h"

int main(int argc, char *argv[]){
    int filenum;
    if (argc == 1){
        printf("No files mentioned. Exiting...\n");
        exit(1);
    }
    printf("\n");
    for (filenum = 1; filenum < argc; filenum++) {

        /* preassemble the current file */
        int line;
        char *curFileName = argv[filenum];
        int errCount = preAssemble(curFileName, &line);
        if (errCount != 0) {
            printf("Total %d errors were found during pre-assemble. Continuing to next file...\n\n", errCount);
            continue;
        }

        /* init lists */
        NAListNode *symbolTable = NULL, *entryList = NULL, *externUseList = NULL;
        dataListNode *dataTable = NULL, *instructionTable = NULL;
        int instructionCounter, dataCounter;


        /*
         * Pass one
         * Build symbolList and dataList
         */
        errCount = pass1(curFileName, &instructionCounter, &dataCounter, &line, &symbolTable, &dataTable);
        if (errCount != 0) {
            printf("Total %d errors were found during pass one. Continuing to next file...\n\n", errCount);
            freeLists(symbolTable, dataTable);
            exit(errCount);
        }

        /*
         * Pass two
         * build entryList, externList, and instructionTable
         */
        errCount = pass2(curFileName, &line, symbolTable, &entryList, &externUseList, &instructionTable);
        if (errCount != 0) {
            printf("Total %d errors were found during pass two. Continuing to next file...\n\n", errCount);

            freeNAList(symbolTable);
            freeNAList(entryList);
            freeNAList(externUseList);
            freeDataNodeList(dataTable);
            freeDataNodeList(instructionTable);

            continue;
        }

        /*
         * Compile the lists into the output files
         */
        compile(curFileName, instructionCounter, dataCounter, instructionTable, dataTable, entryList, externUseList);

        /* Free lists */
        freeNAList(symbolTable);
        freeNAList(entryList);
        freeNAList(externUseList);
        freeDataNodeList(dataTable);
        freeDataNodeList(instructionTable);

        printf("Successfully compiled file \"%s\"!\n\n", strcat(curFileName, ".as"));
    }

    printf("Finished compiling all files!\n");

    return 0;
}


