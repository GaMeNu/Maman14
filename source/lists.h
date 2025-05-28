#ifndef MAMAN14_2_LISTS_H
#define MAMAN14_2_LISTS_H

enum {INST_WORD, DATA_WORD};

typedef struct nameAddressPair{
    char *name;
    int address;
    char type;
} namedAddress;

typedef struct nameAddressList{
    namedAddress *entry;
    struct nameAddressList *next;
} NAListNode;

typedef struct dataNode{
    unsigned int data:12;
} dataNode;

typedef struct instructionNode{
    unsigned char encoding:2;
    unsigned char destOp:3;
    unsigned char opCode:4;
    unsigned char srcOp:3;
} instructionNode;

typedef struct dataListNode{
    int dataType;
    union{
        instructionNode *instWord;
        dataNode *dataWord;
    };
    struct dataListNode *next;
} dataListNode;

/**
 * Create new namedAddress node
 * @param name namedAddress key (label name)
 * @param address namedAddress value (label address)
 * @param type label type
 * @return pointer to namedAddress (NULL if failed)
 */
namedAddress* newEntry(char* name, int address, char type);
/**
 * Free a namedAddress from memory
 * @param entry pointer to namedAddress
 */
void freeEntry(namedAddress* entry);

/**
 * Create a listNode containing an entry
 * @param entry
 * @return
 */
NAListNode *createNAListNode(namedAddress *entry);
/**
 * Get a namedAddress from a table by its name
 * @param head table head
 * @param name label name
 * @return pointer to namedAddress
 */
namedAddress * getNamedAddress(NAListNode *head, char* name);
/**
 * Appends a namedAddress to the end of the list
 * @param head list head
 * @param entry namedAddress to append
 */
void appendNAListNode(NAListNode ** head, namedAddress *entry);

/**
 * Add a certain value to each of the addresses
 * @param head list head
 * @param addressShift value to shift addresses by
 * @param ... address types
 */
void updateAddresses(NAListNode *head, int addressShift, char type);

/**
 * Frees an entire NAList from memory
 * @param head list head
 */
void freeNAList(NAListNode* head);
/**
 * @deprecated
 *
 * Print an NA list to terminal (was used for testing)
 * @param head list head
 */
void printNAList(NAListNode *head);


/**
 * Create a new DATA node
 * @param data int with up to 12 bits
 * @return node pointer
 */
dataNode *newDataNode(int data);
/**
 * Create a new INSTRUCTION node
 * @param encoding A,R,E (2 bits)
 * @param destOp destination operand addressing (3 bits)
 * @param opCode operation code (4 bits)
 * @param srcOp source operand addressing (3 bits)
 * @return pointer to instructionNode
 */
instructionNode *newInstrcutionNode(char encoding, char destOp, char opCode, char srcOp);
/**
 * Appends a DATA node to a dataListNode
 * @param head list head
 * @param node node to append
 */
void appendDataNode(dataListNode **head, dataNode *node);
/**
 * Appends an INSTRUCTION node to a dataListNode
 * @param head list head
 * @param node node to append
 */
void appendInstNode(dataListNode **head, instructionNode *node);
/**
 * Frees a dataListNode list
 * @param head list head
 */
void freeDataNodeList(dataListNode *head);
/**
 * @deprecated
 *
 * Print a dataList to terminal (was used for testing)
 * @param head list head
 */
void printDataList(dataListNode *head);
/**
 * @deprecated
 *
 * print a binary number to terminal (was used for testing)
 * newline not included
 * @param number number to print
 */
void printBinaryNumber(unsigned char number);

void printDataNode(dataNode *node);
void printInstNode(instructionNode *node);

void freeLists(NAListNode *NAHead, dataListNode *dataHead);

int isSymbol(char *word);

#endif
