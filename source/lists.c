#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"
#include "statements.h"

NAListNode* createNAListNode(namedAddress* entry) {
    NAListNode* newNode = (NAListNode*)malloc(sizeof(NAListNode));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->entry = entry;
    newNode->next = NULL;
    return newNode;
}

void appendNAListNode(NAListNode** head, namedAddress* entry) {
    NAListNode* newNode = createNAListNode(entry);

    if (*head == NULL) {
        *head = newNode;
    } else {
        NAListNode* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

namedAddress* getNamedAddress(NAListNode* head, char* name) {
    NAListNode* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->entry->name, name) == 0) {
            return temp->entry;
        }
        temp = temp->next;
    }
    return NULL;
}


namedAddress *newEntry(char *name, int address, char type) {
    namedAddress* entry = (namedAddress*)malloc(sizeof(namedAddress));
    if (entry == NULL) {
        return NULL;
    }

    entry->name = strdup(name);
    if(entry->name == NULL){
        free(entry);
        return NULL;
    }

    entry->address = address;
    entry->type = type;
    return entry;
}

void freeEntry(namedAddress *entry) {
    free(entry->name);
    free(entry);
}

void printNAList(NAListNode *head) {
    NAListNode * current = head;
    char type[3];
    while (current != NULL){
        switch (current->entry->type) {
            case DATA:
                strcpy(type, "D");
                break;
            case EXTERNAL:
                strcpy(type, "EX");
                break;
            case CODE:
                strcpy(type, "C");
                break;
            case ENTRY:
                strcpy(type, "EN");
                break;
        }
        printf("%s [%s] : %d\n", current->entry->name, type, current->entry->address);
        current = current->next;
    }
}

void freeNAList(NAListNode *head) {
    while (head != NULL) {
        NAListNode* temp = head;
        head = head->next;
        freeEntry(temp->entry);
        free(temp);
    }
}


void updateAddresses(NAListNode *head, int addressShift, char type) {
    NAListNode * current = head;
    while (current != NULL){
        if(current->entry->type == type){
            current->entry->address += addressShift;
        }
        current = current->next;
    }
}

dataNode *newDataNode(int data) {
    dataNode *result = malloc(sizeof(dataNode));
    result->data = data;
    return result;
}

instructionNode *newInstrcutionNode(char encoding, char destOp, char opCode, char srcOp) {
    instructionNode *inst = malloc(sizeof(instructionNode));
    if(inst == NULL) return NULL;
    inst->encoding = encoding;
    inst->destOp = destOp;
    inst->opCode = opCode;
    inst->srcOp = srcOp;
    return inst;
}

void appendDataNode(dataListNode **head, dataNode *node) {
    dataNode * newNode = node;
    if ((*head) == NULL) {
        *head = malloc(sizeof(dataListNode));
        (*head)->next = NULL;
        (*head)->dataWord = node;
        (*head)->dataType = DATA_WORD;
    } else {
        dataListNode * temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = malloc(sizeof(dataListNode));
        temp->next->next = NULL;
        temp->next->dataWord = newNode;
        temp->next->dataType = DATA_WORD;
    }
}

void appendInstNode(dataListNode **head, instructionNode *node) {
    instructionNode * newNode = node;
    if ((*head) == NULL) {
        *head = malloc(sizeof(dataListNode));
        (*head)->next = NULL;
        (*head)->instWord = node;
        (*head)->dataType = INST_WORD;
    } else {
        dataListNode * temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = malloc(sizeof(dataListNode));
        temp->next->next = NULL;
        temp->next->instWord = newNode;
        temp->next->dataType = INST_WORD;
    }
}

void freeDataNodeList(dataListNode *head) {
    while (head != NULL) {
        dataListNode *temp = head;
        head = head->next;

        if(temp->dataType == INST_WORD){
            free(temp->instWord);
        } else {
            free(temp->dataWord);
        }
        free(temp);
    }
}

void printDataList(dataListNode *head) {
    dataListNode *current = head;

    while (current != NULL){
        if (current->dataType == INST_WORD){
            printInstNode(current->instWord);
        } else {
            printDataNode(current->dataWord);
        }
        current = current->next;
    }
}

void freeLists(NAListNode *NAHead, dataListNode *dataHead) {
    freeNAList(NAHead);
    freeDataNodeList(dataHead);
}

/**
 * Checks whether a string is a label definition
 * @param word token to check (includes ':' character)
 * @return 1 for true, 0 for false
 */
int isSymbol(char *word){
    if(word[0] < 'A' || word[0] > 'z') return 0;
    if (word[strlen(word)-1] != ':') return 0;
    return 1;
}

/**
 * Prints a binary representation of a number to the terminal
 * @param number number to print
 */
void printBinaryNumber(unsigned char number) {
    char buffer[33];
    /*itoa(number, buffer, 2);*/
    buffer[12] = '\0';
    printf("%s", buffer);
}

/**
 * Prints an instruction node to the terminal
 * (splits bits, no 0 left-padding)
 * @param node instruction node to print
 */
void printInstNode(instructionNode *node) {
    printf("\n");
    printBinaryNumber(node->srcOp);
    printf(" ");
    printBinaryNumber(node->opCode);
    printf(" ");
    printBinaryNumber(node->destOp);
    printf(" ");
    printBinaryNumber(node->encoding);
    printf("\n");
}


/**
 * prints a data node representation to the terminal
 * (no splitting, includes 0 left-padding)
 * @param node node to print
 */
void printDataNode(dataNode *node) {
    char buffer[33];
    int len;
    int i;
    /*itoa(node->data, buffer, 2);*/
    buffer[12] = '\0';
    len = strlen(buffer);
    for (i = 12 - len; i > 0; i--){
        printf("0");
    }

    printf("%s", buffer);
    printf("\n");
}
