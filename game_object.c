#include <stddef.h>
// We have to use this one for malloc because we are weak
#include <stdlib.h>
#include "game_object.h"

void init(LinkedList list) {
    list.head = NULL;
    list.tail = NULL;
    list.size = 0;
}

void insert(LinkedList list, void *pt) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->value = pt;
    node->next = NULL;
    if (list.head == NULL) {
        list.head = node;
    } else {
        list.tail->next = node;
        list.tail = node;
    }
    list.size++;
}

Node* getElementAt(LinkedList list, int index) {
    int i = 0;
    Node *current = list.head;

    if (list.head == NULL) return NULL;
    while (i != index && current != NULL) {
        current = current->next;
        i++;
    }
    //Either null or something
    return current;
}

Node* removeAt(LinkedList list, int index) {
    int i = 0;
    Node *current = list.head;
    Node *prevNode = NULL;

    if (list.head == NULL) return NULL;
    if (index == 0) {
        prevNode = list.head;
        list.head = list.head->next;
        list.size--;
        return prevNode;
    }
    while (i != index && current != list.tail) {
        prevNode = current;
        current = current->next;
        i++;
    }
    if (i == index) {
        prevNode->next = current->next;
        list.size--;
        return current;
    }
    return NULL;
}

void freeList(LinkedList list) {
    Node *node = list.head;
    Node *prevNode = NULL;
    while (node != NULL) {
        free(node->value);
        prevNode = node;
        node = node->next;
        free(node);
    }
    free(prevNode);
}

// Method to find the index'th true value in a binary map
// Return -1 if an error was found
int findBinaryMapping(int binaryMap[], int index, int length) {
    int i = 0;
    for (;i < length && index > 0;i++) {
        if (binaryMap[i] >= 1) index--;
    }
    if (index == 0) {
        return i;
    }
    return -1;
}

//TODO: this can overflow
void addTimeOfTheDay(Game *game, int value) {
    game->timeOfTheDay += value;
    if (game->timeOfTheDay > game->config.maxTimeOfTheDay) {
        game->timeOfTheDay -= game->config.maxTimeOfTheDay;
        game->day++;
    }
}