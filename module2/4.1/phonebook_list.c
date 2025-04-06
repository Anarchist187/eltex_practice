#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

void menu()
{
    printf("Choose action: \n");
    printf("1.Enter person \n");
    printf("2.Get list of persons \n");
    printf("3.Delete person \n");
    printf("4.Edit person \n");
    printf("Your choice:");   
}

struct person
{
    int personID;
    int additionalFields;
    char firstName [20];
    char secondName[20];
    char phoneNumb[20];
    char emailAddr[20];
}
p;

typedef struct List{
    struct person pList;
    struct List *pNext;
    struct List *pPrev;
}
List;

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

int main()
{
    struct List* head=NULL;
    int count = 0;
    int action = 0;
    int indexId = 0;
    char pfirstName[20];
    char plastName[20];
    char pphoneNumb[20];
    char pemailAddr[20];
   // setlocale(LC_ALL, "Russian");
while (1){
    menu();
    scanf("%d", &action);
    switch (action)
    {
        case 1:
        {
            printf("\nEnter first name: \n");
            scanf("%s", &pfirstName);
            fseek(stdin, 0, SEEK_END);
            printf("Enter second name: \n");
            scanf("%s", &plastName);
            fseek(stdin, 0, SEEK_END);
            addPoint:
            printf("Wanna fill in additional fields? (1-y; 2-n) \n");
            scanf("%d", &indexId);
            fseek(stdin, 0, SEEK_END);
            if (indexId==1)
            {
                printf("Enter phone number: \n");
                scanf("%s", &pphoneNumb);
                printf("Enter email: \n");
                scanf("%s", &pemailAddr);
            }
            else if (indexId != 2)
            {
                printf("Not an option\n");
                goto addPoint;
            }
            head=insertPerson(head,count, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            count++;
            break;
        }
        case 2:
        {
            printList(head);
            break;
        }
        case 3:
        {
            printf("Enter person-to-delete ID: ");
            scanf("%d", &indexId);
            fseek(stdin, 0, SEEK_END);
            if (indexId>count-1){
                printf("No person with same ID\n");
                break;
            }
            head=deletePersonList(indexId, head);
            count--;
            head=rearrangeID(head,count);
            break;
        }
        case 4:
        {
            printf("Enter person-to-edit ID: ");
            scanf("%d", &action);
            fseek(stdin, 0, SEEK_END);
            if (action>count-1){
                printf("No person with same ID\n");
                break;
            }
            printf("\nEnter first name: \n");
            scanf("%s", &pfirstName);
            printf("Enter second name: \n");
            scanf("%s", &plastName);
            editPoint:
            printf("Wanna edit additional fields? (1-y 2-n)");
            scanf("%d", &indexId);
            if (indexId==1)
            {
                printf("\nEnter phone: \n");
                scanf("%s", &pphoneNumb);
                printf("Enter email: \n");
                scanf("%s", &pemailAddr);
            }
            else if (indexId!=2)
            {
                printf("Not an option\n");
                goto editPoint;
            }
            head=editPersonList(head,action, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            break;
        }
        default:{
            deleteList(head);
            return 0;
        }
    }
}
}