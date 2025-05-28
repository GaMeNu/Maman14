
#ifndef MAMAN14_2_COMPILATION_H
#define MAMAN14_2_COMPILATION_H

/**
 * Compile the program into the final output files
 * @param filename output filename (without extension)
 * @param instructionTable the instruction table
 * @param dataTable the data table
 * @param symbolTable the label table
 * @return exit code (0 for success)
 */
int compile(char *filename, int ic, int dc, dataListNode *instructionTable, dataListNode *dataTable, NAListNode *entryTable, NAListNode *externTable);

/**
 * Get a value's matching Base64 char
 * @return a single Base64 char representing the input value
 */
char getMatchingChar(char val);

/**
 * Writes a dataList to a file
 * @param dataTable listNode
 * @param outputFile file to write to
 */
void writeListToFile(dataListNode *dataTable, FILE *outputFile);

/**
 * Writes the .ext and .ent files
 * @param filename filename (without file extension)
 * @param externalsTable Head of externals table
 * @param entryTable Head of entries table
 */
int writeSymbolsFiles(char *filename, NAListNode *entryTable, NAListNode *externalsTable);

/**
 * Creates a new file if it isn't open.
 * @param entFile file pointer
 * @param entFilename file name
 */
void createNewFile(FILE **entFile, char *entFilename);
#endif
