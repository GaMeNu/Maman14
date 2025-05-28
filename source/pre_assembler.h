#ifndef MAMAN14_2_PRE_ASSEMBLER_H
#define MAMAN14_2_PRE_ASSEMBLER_H

#define LINE_SIZE 81
#define MAX_WORD_SIZE 32

typedef struct macro{
    char *name;
    char *value;
} macro;

typedef struct macroList{
    macro *macro;
    struct macroList *next;
} macroList;

macroList *initMacroList();
macro *newMacro(char *name, char *content);
void appendMacro(macro *mcro, macroList **table);
void printTable(macroList *macrolist);
macro *getMacro(macroList *head, char *macroName);
void freeMacro(macro *mcro);
void freeMacroList(macroList *list);
int preAssemble(char *filename, int *line);

#endif
