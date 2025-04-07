#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contacts.h"

void menu()
{
    printf("Choose action: \n");
    printf("1.Enter person \n");
    printf("2.Get list of persons \n");
    printf("3.Delete person \n");
    printf("4.Edit person \n");
    printf("Your choice:");   
}
struct List *insertPerson(struct List* head, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr){
    struct List* newItem;
    struct List* tmp = head;
    newItem=malloc(sizeof(struct List));
    newItem->pList.personID=ID;
    newItem->pList.additionalFields = addFld;
    strcpy( newItem->pList.firstName, firstName);
    strcpy( newItem->pList.secondName, secondName);
    if (addFld)
    {
       strcpy( newItem->pList.phoneNumb, phoneNumb);
       strcpy( newItem->pList.emailAddr, emailAddr);
    }
    if (head==NULL){
        head=newItem;
        newItem->pNext=newItem;
        newItem->pPrev=newItem;
        return head;
    }
    do{
        if(newItem->pList.personID<=tmp->pList.personID){
            newItem->pNext=tmp;
            newItem->pPrev=tmp->pPrev;
            tmp->pPrev->pNext=newItem;
            tmp->pPrev=newItem;
            if (tmp == head) 
                head=newItem;
                return head;
        }
        tmp =tmp->pNext;
    } while(tmp!=head);
        newItem->pNext=tmp;
        newItem->pPrev=tmp->pPrev;
        tmp->pPrev->pNext=newItem;
        tmp->pPrev=newItem;
        return head;
}

struct List* deleteList(struct List* head){
    struct List* tmp = head;
    struct List* removeItem;
    if (NULL==head)
        return NULL;
    do {
        removeItem=tmp;
        tmp=tmp->pNext;
        free(removeItem);
    }while(tmp!=head);
    return NULL;
}

struct List* deletePersonList(int ID,struct List* head){
    struct List* tmp=head;
    if (head==NULL)
        return head;
    do{
        if (ID==tmp->pList.personID){
            if (tmp->pNext==tmp->pPrev){
                free(tmp);
                return NULL;
            }
            tmp->pPrev->pNext=tmp->pNext;
            tmp->pNext->pPrev=tmp->pPrev;
            if(tmp==head)
                head=head->pNext;
            free (tmp);
            return head;
        }
        tmp=tmp->pNext;
    } while (tmp!=head);
    return head;
}

struct List* rearrangeID(struct List* head, int count){
    int newID=0;
    struct List* tmp =head;
    if (head==NULL)
        return head;
    do{
        tmp->pList.personID=newID;
        tmp=tmp->pNext;
        newID++;
    }while (newID!=count);
    return head;
}

void printList (struct List *head){
    struct List* tmp =head;
    if (head==NULL)
        return;
    do{
        printf("ID: %d\n", tmp->pList.personID);
        printf("First Name: %s\n", tmp->pList.firstName);
        printf("Last Name: %s\n", tmp->pList.secondName);
        if (tmp->pList.additionalFields==1){
            printf("Phone Number: %s\n", tmp->pList.phoneNumb);
            printf("Email Address: %s\n", tmp->pList.emailAddr);
        }
        tmp=tmp->pNext;
    }while (tmp!=head);
}
struct List* editPersonList(struct List* head, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr){
    struct List* tmp =head;
    do{
        if (ID==tmp->pList.personID){
            strcpy( tmp->pList.firstName, firstName);
            strcpy( tmp->pList.secondName, secondName);
        if (addFld){
            strcpy( tmp->pList.phoneNumb, phoneNumb);
            strcpy( tmp->pList.emailAddr, emailAddr);
        } 
        return head;
        }
        tmp=tmp->pNext;
    } while (tmp!=head);
}