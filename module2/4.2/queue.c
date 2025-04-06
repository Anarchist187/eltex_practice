#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct Node{
    int priority;
    int value;
    struct Node *pNext; 
}Node;

typedef struct{
    Node *head;
}Queue;

void initQueue(Queue *queue){
    queue->head=NULL;
}

void addToQueue(Queue *queue, int priority, int value){
    Node *newNode=(Node*)malloc(sizeof(Node));
    newNode->priority=priority;
    newNode->value=value;
    newNode->pNext=NULL;

    if (queue->head==NULL || newNode->priority<queue->head->priority){
        newNode->pNext=queue->head;
        queue->head=newNode;
    }else{
        Node *elem=queue->head;
        while(elem->pNext->priority<=newNode->priority && elem->pNext!=NULL)
            elem=elem->pNext;
        newNode->pNext=elem->pNext;
        elem->pNext=newNode;
    }
}

Node *extractMaxPriority(Queue *queue){
    if (queue->head==NULL)
        return NULL;
    Node *tmp=queue->head;
    queue->head=tmp->pNext;
    return tmp;
}

Node *extractExactPriority(Queue *queue, int priority){
    if (queue->head==NULL)
        return NULL;
    if (queue->head->priority==priority){
        Node *elem=queue->head;
        queue->head=elem->pNext;
        return elem;
    }
    Node *elem=queue->head;
    while(elem->pNext->priority!=priority && elem->pNext!=NULL)
        elem=elem->pNext;
    if (elem->pNext==NULL)
        return NULL;
    Node *result=elem->pNext;
    elem->pNext=(result->pNext!=NULL ? result->pNext : NULL);
    return result;
}

Node *extractGreaterPriority(Queue *queue, int priority){
    if (queue->head==NULL)
        return NULL;
    if (queue->head->priority<=priority){
        Node *elem=queue->head;
        queue->head=elem->pNext;
        return elem;
    }
    if (queue->head->priority<=priority)
        return queue->head;
    Node *elem=queue->head;
    while(elem->pNext->priority>=priority && elem->pNext!=NULL)
        elem=elem->pNext;
    if (elem->pNext==NULL)
        return NULL;
    Node *result=elem->pNext;
    elem->pNext=(result->pNext!=NULL ? result->pNext : NULL);
    return result;
}

void releaseNode(Node *queue){
    if (queue!=NULL)
    free (queue);
}

void listQueue(Queue *queue){
    if (queue->head==NULL){
        printf("Queue is empty!\n");
        return;
    }
    printf("Priority:   Value:\n");
    Node *elem=queue->head;
    do{
        printf("%d           %d\n",elem->priority,elem->value);
        elem=elem->pNext;
    }while(elem!=NULL);
}

int main(){
    Queue queue;
    initQueue(&queue);
    addToQueue(&queue,6,100);
    addToQueue(&queue,2,150);
    addToQueue(&queue,2,120);
    addToQueue(&queue,2,300);
    addToQueue(&queue,1,50);
    addToQueue(&queue,4,200);
    addToQueue(&queue,2,30);
    listQueue(&queue);
    printf("Extracting element with highest priority:\n");
    Node *elem = extractMaxPriority(&queue);
    printf("Extracted element: Priority: %d, Value: %d\n",elem->priority,elem->value);
    releaseNode(elem);
    printf("Current queue:\n");
    listQueue(&queue);
    printf("Extracting element with certain priority:\n");
    elem = extractExactPriority(&queue, 2);
    printf("Extracted element: Priority: %d, Value: %d\n",elem->priority,elem->value);
    releaseNode(elem);
    printf("Current queue:\n");
    listQueue(&queue);
    printf("Extracting element with greater priority:\n");
    elem = extractGreaterPriority(&queue,4);
    printf("Extracted element: Priority: %d, Value: %d\n",elem->priority,elem->value);
    releaseNode(elem);
    printf("Current queue:\n");
    listQueue(&queue);
    
}

