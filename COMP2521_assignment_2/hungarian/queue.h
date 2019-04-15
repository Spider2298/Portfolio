#ifndef QUEUE_H
#define QUEUE_H

typedef struct node {
    int value;
    struct node* next;    
} *Node;


typedef struct queue {
    Node head;
    Node tail;
    
} *Queue;

Queue genQueue(void);
void enterQueue(Queue q, int value);
int leaveQueue(Queue q);
int emptyQueue(Queue q);
void dropQueue(Queue q);


#endif
