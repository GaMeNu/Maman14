#ifndef MAMAN14_2_ERRORS_H
#define MAMAN14_2_ERRORS_H

#define ERROR_NAME_SIZE 32
#define ERROR_MESSAGE_BUFFER_SIZE 256

#define FILE_ERROR 100
#define MEM_ERROR 200
#define MISSING_SYMBOL_INTERNAL_ERROR 201
#define INVALID_OPERAND_ADDRESSING_INTERNAL_ERROR 202
#define SYNTAX_ERROR 300
#define UNDECLARED_MACRO_ERROR 301
#define DOUBLE_MACRO_ERROR 302
#define REDEFINED_MACRO_ERROR 303
#define UNENDED_MACRO_ERROR 304
#define RESERVED_NAME_ERROR 400
#define REDEFINED_SYMBOL_ERROR 401
#define UNDEFINED_SYMBOL_ERROR 402
#define INVALID_OPERAND_ERROR 403
#define INSTRUCTION_NOT_FOUND_ERROR 404
#define INVALID_INSTRUCTION_ERROR 405
#define OPERAND_COUNT_ERROR 406
#define INVALID_OPERAND_TYPE_ERROR 407

/**
 * returns the error code's name
 * @param code error code
 * @return error code's name
 */
char *errorCodeToName(int code);

/**
 * Throws an error to the terminal
 * @param code error code
 * @param line line error has occurred on
 * @param filename file error has occurred in
 * @param message custom output message
 */
void throwError(int code, int line, char *filename, char *message);


#endif
