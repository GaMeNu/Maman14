
#ifndef MAMAN14_2_INSTRUCTIONS_H
#define MAMAN14_2_INSTRUCTIONS_H

#define LINE_SIZE 81
#define MAX_WORD_SIZE 32

/**
 * Process the entire instruction
 * @param line an entire instruction line as a char array
 * @param pDataList pointer to pointer to the data list to add the new data into
 * @param symbolTable pointer to the table of all symbols built on pass one
 * @param ic pointer to the current instruction counter
 * @param ec pointer to error counter
 * @param inputFileName current file name
 * @param lineNum current line number
 * @return error code (0 for success)
 */
int process_instruction(char *line, dataListNode **pDataList, NAListNode *symbolTable, NAListNode **externalsTable, int *ic, int *ec, char *inputFileName, int lineNum);

/**
 * Process a single operand
 * @param operand the operand as a char array
 * @param operandType operand's number
 * @param instruction the instruction that the operand is associated with
 * @param output pointer to set output value into
 * @param pAddressing pointer to set operand addressing type into
 * @param pSymbolType pointer to set operand symbol type (if is symbol, otherwise sets to 0)
 * @param pSymbolName pointer to set operand symbol name (if applicable)
 * @param symbolTable pointer to the table of all symbols built on pass one
 * @param errMsgBuffer error message buffer. will be set if an error occurs
 * @return error code
 */
int process_operand(char *operand, int operandType, char *instruction, int *output, char *pAddressing,
                    char *pSymbolType, char *pSymbolName, NAListNode *symbolTable, char *errMsgBuffer);

/**
 * encode an operand's data into binary
 * @param input data to encode
 * @param addressing operand's addressing
 * @param labelType the label's type, if is a label
 * @param operandType whether operand is number, label, or register
 * @return the encoded data as an int
 */
int assembleData(int input, char addressing, char labelType, char operandType);


#endif
