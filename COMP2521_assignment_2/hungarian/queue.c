#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Queue genQueue(void) {
    Queue newQueue = malloc(sizeof(struct queue));
    assert(newQueue);
    newQueue->head = NULL;
    newQueue->tail = NULL;
    return newQueue;
}

Node genNode(int value) {
    Node newNode = malloc(sizeof(struct node));
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

void enterQueue(Queue q, int value) {
    Node newNode = genNode(value);
    if (q->head == NULL) {
       q->head = newNode;
       q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
}

int leaveQueue(Queue q) {
    assert(q->head);
    if (q->head == q->tail) {
        int value = q->head->value;
        free(q->head);
        q->head = NULL;
        q->tail = NULL;
        return value;
    } else {
        int value = q->head->value;
        Node to_free = q->head;
        q->head = q->head->next;
        free(to_free);
        return value;
    }
}

int emptyQueue(Queue q) {
    if (q->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void dropNodes(Node curr) {
    if (curr == NULL) return;
    dropNodes(curr->next);
    free(curr);
}


void dropQueue(Queue q) {
    dropNodes(q->head);
    free(q);
}




