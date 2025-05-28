#include "errors.h"
#include "lists.h"
#include "whitespaces.h"
#include "statements.h"
#include "instructions.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef struct operand{
    char *op;
    int output;
    char addressing;
    char labelType;
    int assembledData;
} operand;

int process_instruction(char *line, dataListNode **pDataList, NAListNode *symbolTable, NAListNode **externalsTable, int *ic, int *ec, char *inputFileName, int lineNum) {
    char lineCpy[LINE_SIZE];
    char *operands[2] = {NULL, NULL};
    namedAddress *oplabel;
    int opRegNum0 = -1;
    int opRegNum1 = -1;
    char *endptr = NULL;
    namedAddress *nEntry;
    int opNum1, opNum2;
    char errMsgBuffer[ERROR_MESSAGE_BUFFER_SIZE];
    char *extOp;
    char symbolNames[2][MAX_WORD_SIZE];

    strcpy(lineCpy, line);

    char *op = strtok(lineCpy, " ");
    removeWhitespaces(op);

    int opcode = instructionCode(op);
    int operandCount = instructionOpCount(op);

    (*ic)++;

    int curOp;

    /* split operands into array */
    for(curOp = 0; curOp < operandCount; curOp++){
        operands[curOp] = strtok(NULL, ",");
        if (!operands[curOp]){

            throwError(OPERAND_COUNT_ERROR, lineNum, inputFileName, "Too little operands.");
            (*ec)++;
        } else {
            removeWhitespaces(operands[curOp]);
        }
    }

    /* Check for extraneous operand */
    extOp = strtok(NULL, ",");
    if (extOp != NULL){
        removeWhitespaces(extOp);
        strcpy(errMsgBuffer, "Extraneous operand \"");
        strcat(errMsgBuffer, extOp);
        strcat(errMsgBuffer, "\".");

        throwError(OPERAND_COUNT_ERROR, lineNum, inputFileName, errMsgBuffer);
        (*ec)++;
    }

    /* Check for missing operand */
    if (operandCount > 0 && operands[operandCount-1] == NULL) {
        throwError(OPERAND_COUNT_ERROR, lineNum, inputFileName, "Missing operand(s).");
        (*ec)++;
    }

    int errCode;
    operand srcOp, destOp;

    /* handle first operand) */
    if (operands[0] != NULL){

        if (operandCount == 2){
            /* If we're here, first operand is a SOURCE operand */
            srcOp.op = operands[0];

            errCode = process_operand(operands[0], SOURCE, op, &srcOp.output, &srcOp.addressing, &srcOp.labelType,
                                      symbolNames[0], symbolTable, errMsgBuffer);
            if (errCode != 0){
                throwError(errCode, lineNum, inputFileName, errMsgBuffer);
            }

        } else {
            /* If we're here, first operand is a DESTINATION operand (and there are no more operands) */
            srcOp.addressing = NONE; /* Reset addressing for instructionNode */

            destOp.op = operands[0];

            errCode = process_operand(operands[0], DESTINATION, op, &destOp.output, &destOp.addressing, &destOp.labelType,
                                      symbolNames[1], symbolTable, errMsgBuffer);
            if (errCode != 0){
                throwError(errCode, lineNum, inputFileName, errMsgBuffer);
                (*ec)++;
            }
        }
        (*ic)++;
    }

    if (operands[1] != NULL){
        /* Handle second operand (must be a DESTINATION operand) */
        destOp.op = operands[1];
        errCode = process_operand(operands[1], DESTINATION, op, &destOp.output, &destOp.addressing, &destOp.labelType,
                                  symbolNames[1], symbolTable, errMsgBuffer);
        if (errCode != 0){
            throwError(errCode, lineNum, inputFileName, errMsgBuffer);
            (*ec)++;
        }
        (*ic)++;
    }

    /* Handle NO operands */
    if (!operands[0]) {
        /* Reset addressings for instruction node */
        srcOp.addressing = NONE;
        destOp.addressing = NONE;
    }

    int address;
    if (operandCount == 1) address = (*ic) + 100 - operandCount-1;
    else address = (*ic) + 100 - operandCount;

    /* Append externs to extern list */
    if (srcOp.addressing == DIRECT && srcOp.labelType == EXTERNAL){
        nEntry = newEntry(symbolNames[0], (address), EXTERNAL);
        if (nEntry == NULL){
            throwError(MEM_ERROR, lineNum, inputFileName, "Memory allocation failed. Fatal error.");
            return ((*ec) + 1);
        }
        appendNAListNode(externalsTable, nEntry);
    }

    /* dest op is extern */
    if (destOp.addressing == DIRECT && destOp.labelType == EXTERNAL){
        nEntry = newEntry(symbolNames[1], (address+1), EXTERNAL);
        if (nEntry == NULL){
            throwError(MEM_ERROR, lineNum, inputFileName, "Memory allocation failed. Fatal error.");
            return ((*ec) + 1);
        }
        appendNAListNode(externalsTable, nEntry);
    }

    /* Start encoding into the list */
    /* assemble instruction node */
    instructionNode *instNode = newInstrcutionNode(0, destOp.addressing, (char) opcode, srcOp.addressing);
    if (!instNode) {
        throwError(MEM_ERROR, lineNum, inputFileName, "Memory allocation failed. Fatal error.");
        return ((*ec) + 1);
    }
    appendInstNode(pDataList, instNode);
    if (operandCount == 0) return 0;

    dataNode *newNode;

    /*
     * No need to assemble an operand that doesn't exist anyway
     */
    if(operandCount >= 1)
        destOp.assembledData = assembleData(destOp.output, destOp.addressing, destOp.labelType, DESTINATION);
    if (operandCount == 2)
        srcOp.assembledData = assembleData(srcOp.output, srcOp.addressing, srcOp.labelType, SOURCE);


    if (operandCount == 2){

        /*
         * Push the first operand into the list
         */
        newNode = newDataNode(srcOp.assembledData);
        if (!newNode) {
            throwError(MEM_ERROR, lineNum, inputFileName, "Memory allocation failed. Fatal error.");
            return ((*ec) + 1);
        }

        appendDataNode(pDataList, newNode);

        /*
         * Special case for BOTH operands are registers
         * OR their data and save it to the already existing operand's word
         * then return to prevent appending the destination operand
         */
        if (srcOp.addressing == DIRECT_REGISTER && destOp.addressing == DIRECT_REGISTER){
            /* If both operands are registers and share one data word */
            newNode->data = (newNode->data | destOp.assembledData);
            (*ic)--;
            return 0;
        }

    }

    /*
     * append destination operand
     */
    newNode = newDataNode(destOp.assembledData);
    if (!newNode) {
        throwError(MEM_ERROR, lineNum, inputFileName, "Memory allocation failed. Fatal error.");
        return ((*ec) + 1);
    }
    appendDataNode(pDataList, newNode);


    return 0;

}

int process_operand(char *operand,
                    int operandType,
                    char *instruction,
                    int *output,
                    char *pAddressing,
                    char *pSymbolType,
                    char *pSymbolName,
                    NAListNode *symbolTable,
                    char *errMsgBuffer) {
    int opRegNum, opNum;
    char *endptr;
    char *errMsgPtr = errMsgBuffer;
    if (operand[0] == '@'){
        if (operand[1] != 'r') {
            strcpy(errMsgPtr, "Invalid register \"");
            strcat(errMsgPtr, operand);
            strcat(errMsgPtr, "\".\nRegisters must begin with \"@r\", followed by a number.");
            return INVALID_OPERAND_ERROR;
        }

        if (operand[3] != '\0') {
            strcpy(errMsgPtr, "Invalid register \"");
            strcat(errMsgPtr, operand);
            strcat(errMsgPtr, "\".\nRegisters must begin with \"@r\", followed by a number between 0 and 7.");
            return INVALID_OPERAND_ERROR;
        }

        opRegNum = operand[2] - 48;
        if (opRegNum < 0 || opRegNum > 7) {
            strcpy(errMsgPtr, "Invalid register \"");
            strcat(errMsgPtr, operand);
            strcat(errMsgPtr, "\".\nRegister number must be between 0 and 7");
            return INVALID_OPERAND_ERROR;
        }

        *pAddressing = DIRECT_REGISTER;

        *pSymbolType = 0;
        *output = opRegNum;

    } else {
        opNum = strtol(operand, &endptr, 10);
        if (errno == 0 && !*endptr){
            *pAddressing = IMMEDIATE;
            *pSymbolType = 0;
            *output = opNum;
        } else {

            namedAddress *oplabel = getNamedAddress(symbolTable, operand);
            if(oplabel == NULL){
                /* Found a symbol with no definition */
                strcpy(errMsgPtr, "Label \"");
                strcat(errMsgPtr, operand);
                strcat(errMsgPtr, "\" is undefined.");
                return UNDEFINED_SYMBOL_ERROR;
            }

            *pAddressing = DIRECT;
            *pSymbolType = oplabel->type;
            strcpy(pSymbolName, oplabel->name);
            *output = oplabel->address;

        } /*end ifelse errno == 0 && !*endptr*/
    }/*end ifelse operands[0][0] == '@'*/

    if (operandType == DESTINATION && destAddressingMatches(instruction, *pAddressing) == 0) {
        strcpy(errMsgPtr, "Invalid operand type for operand \"");
        strcat(errMsgPtr, operand);
        strcat(errMsgPtr, "\".");
        return INVALID_OPERAND_TYPE_ERROR;
    }

    if (operandType == SOURCE && srcAddressingMatches(instruction, *pAddressing) == 0) {
        strcpy(errMsgPtr, "Invalid operand type for operand \"");
        strcat(errMsgPtr, operand);
        strcat(errMsgPtr, "\".");
        return INVALID_OPERAND_TYPE_ERROR;
    }
    return 0;
}

int assembleData(int input, char addressing, char labelType, char operandType) {
    int data;
    switch (addressing) {
        case DIRECT_REGISTER:
            if (operandType == SOURCE)
                data = input << 7;
            else
                data = input << 2;
            break;
        case DIRECT:
            data = input << 2;
            if (labelType == EXTERNAL)
                data = data | 1;
            else
                data = data | 2;

            break;
        case IMMEDIATE:
            data = input << 2;
            break;
        default:
            return -1;
    }
    return data;
}
