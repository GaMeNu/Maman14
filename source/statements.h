
#ifndef MAMAN14_2_STATEMENTS_H
#define MAMAN14_2_STATEMENTS_H

#define DIRECTIVE_AMOUNT 4
#define INSTRUCTION_AMOUNT 16

enum symbolTypes {DATA, EXTERNAL, CODE, ENTRY};
enum operandAddressings {NONE = 0, IMMEDIATE = 1, DIRECT = 3, DIRECT_REGISTER = 5};
enum operandTypes {SOURCE = 0, DESTINATION = 1};

/**
 * Checks whether a word is a reserved directive
 * @param word word to check
 * @return 1 for matches, 0 for not
 */
int isDirective(char *word);

/**
 * Checks whether a word is a reserved instruction
 * @param word word to check
 * @return 1 for matches, 0 for not
 */
int isInstruction(char *word);

/**
 * Checks whether a word is a reserved keyword
 * @param word word to check
 * @return 1 for matches, 0 for not
 */
int isReserved(char *word);


/**
 * Returns the instruction code of an operation
 * @param word operation name
 * @return operation code
 */
int instructionCode(char *word);


/**
 * Returns the required operand count of an operation
 * @param word operation name
 * @return operand count (-1 if failed)
 */
int instructionOpCount(char *word);

/**
 * Checks whether the addressing given is a valid SOURCE addressing
 * @param word operation name
 * @param addressing addressing to check
 * @return 1 for matches, 0 for doesn't match
 */
int srcAddressingMatches(char *word, int addressing);

/**
 * Checks whether the addressing given is a valid DESTINATION addressing
 * @param word operation name
 * @param addressing addressing to check
 * @return 1 for matches, 0 for doesn't match
 */
int destAddressingMatches(char *word, int addressing);

#endif
