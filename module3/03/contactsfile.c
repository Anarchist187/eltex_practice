#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

const char *FILE_TO_RW="list.txt";

void menu()
{
    printf("Choose action: \n");
    printf("1.Enter person \n");
    printf("2.Get list of persons \n");
    printf("3.Delete person \n");
    printf("4.Edit person \n");
    printf("0.Exit\n");
    printf("Your choice:");   
}

typedef struct person
{
    int personID;
    char firstName [20];
    char secondName[20];
    char phoneNumb[20];
    char emailAddr[20];
}
person;

typedef struct List{
    person pList;
    struct List *pNext;
    struct List *pPrev;
}
List;

typedef struct linkedList{
    List *head;
    List *tail;
    int pNextId;
} linkedList;

void insertPerson(linkedList* list, char* firstName, char* secondName, char* phoneNumb, char* emailAddr){
    List* newItem=(List*)malloc(sizeof(List));
    newItem->pList.personID=list->pNextId++;
    strncpy( newItem->pList.firstName, firstName,20);
    strncpy( newItem->pList.secondName, secondName,20);
    strncpy( newItem->pList.phoneNumb, phoneNumb,20);
    strncpy( newItem->pList.emailAddr, emailAddr,20);
    newItem->pNext=NULL;
    newItem->pPrev=NULL;   
    if (list->head==NULL)
        list->head=list->tail=newItem;
    else{
        List* tmp = list->head;
        while(tmp != NULL && strcmp(tmp->pList.secondName,secondName)<0){
            tmp=tmp->pNext;
        }
        if (tmp==NULL){
            list->tail->pNext=newItem;
            newItem->pPrev=list->tail;
            list->tail=newItem;
        } else if(tmp == list->head){
            newItem->pNext=list->head;
            list->head->pPrev=newItem;
            list->head = newItem;
        } else{
            newItem->pNext=tmp;
            newItem->pPrev=tmp->pPrev;
            tmp->pPrev->pNext=newItem;
            tmp->pPrev=newItem;
        }
    }   
}
void deleteList(linkedList *list){
    List *tmp = list->head;
    while (tmp!=NULL){
        List *removeItem = tmp->pNext;
        free(tmp);
        tmp=removeItem;
    }
    list->head = list->tail = NULL;
}

void deletePersonList(int ID,linkedList* list){
    List* tmp=list->head;
    if (list->head==NULL)
        return;
    while (tmp!=NULL && tmp->pList.personID != ID){
        tmp = tmp->pNext;
    }
    if (tmp == NULL){
        return;
    }
    if (tmp->pPrev != NULL){
        tmp->pPrev->pNext=tmp->pNext;
    } else{
        list->head = tmp->pNext;
    } 
    if (tmp->pNext != NULL){
        tmp->pNext->pPrev=tmp->pPrev;
    }
    free (tmp);
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

void printList (linkedList *list){
    List* tmp =list->head;
    if (tmp==NULL)
        return;
    while (tmp!=NULL){
        printf("ID: %d\n", tmp->pList.personID);
        printf("First Name: %s\n", tmp->pList.firstName);
        printf("Last Name: %s\n", tmp->pList.secondName);
        printf("Phone Number: %s\n", tmp->pList.phoneNumb);
        printf("Email Address: %s\n", tmp->pList.emailAddr);
        tmp=tmp->pNext;
    }
}
List *findContactWithId(linkedList *list, int ID){
    List *tmp = list->head;
    while (tmp!=NULL){
        if (tmp->pList.personID==ID)
            return tmp;
        tmp=tmp->pNext;
    }
    return NULL;
}

void editPersonList(linkedList *list, int ID, int add_fld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr){
    List *editContact = findContactWithId(list,ID);
    
    if (editContact==NULL){
        printf ("Contact with ID %d not found\n",ID);
        return;
    }
    char editPhoneNumb[20], editEmailAddr[20];
    strncpy(editPhoneNumb,editContact->pList.phoneNumb,20);
    strncpy(editEmailAddr,editContact->pList.emailAddr,20);

    person newItem = editContact->pList;

    strncpy(newItem.firstName,firstName,20);
    strncpy(newItem.secondName,secondName,20);
    if (add_fld==1){
        strncpy(newItem.emailAddr,emailAddr,20);
        strncpy(newItem.phoneNumb,phoneNumb,20);
    }else{
        strncpy(newItem.emailAddr,editEmailAddr,20);
        strncpy(newItem.phoneNumb,editPhoneNumb,20);
    }
    deletePersonList(ID,list);
    List *newItemm=(List*)malloc(sizeof(List));
    newItemm->pList=newItem;
    newItemm->pNext=NULL;
    newItemm->pPrev=NULL; 

    if (list->head==NULL){
        list->head=list->tail=newItemm;
    }else{
        List* tmp = list->head;
        while(tmp != NULL && strcmp(tmp->pList.secondName,secondName)<0){
            tmp=tmp->pNext;
        }
        if (tmp==NULL){
            list->tail->pNext=newItemm;
            newItemm->pPrev=list->tail;
            list->tail=newItemm;
        } else if(tmp == list->head){
            newItemm->pNext=list->head;
            list->head->pPrev=newItemm;
            list->head = newItemm;
        } else{
            newItemm->pNext=tmp;
            newItemm->pPrev=tmp->pPrev;
            tmp->pPrev->pNext=newItemm;
            tmp->pPrev=newItemm;
        }
    }   
}

void writeContactsToFile(linkedList *list) {
    int fd = open(FILE_TO_RW, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        return;
    }

    if (write(fd, &list->pNextId, sizeof(int))!=sizeof(int)){
        perror ("Error writing");
        close(fd);
        return;
    }
    
    List* tmp = list->head;
    while(tmp!=NULL){
        if (write (fd, &tmp->pList,sizeof(person))!=sizeof(person)){
            perror("Error writing contact");
            break;
        }
        tmp=tmp->pNext;
    }
    close(fd);
}

void readContactsFromFile(linkedList* list){
    int fd = open(FILE_TO_RW, O_RDONLY);
    if (fd == -1) {
        list->pNextId=2;
        return;
    }

    if (read(fd, &list->pNextId, sizeof(int)) != sizeof(int)) {
        perror("Error reading id");
        close(fd);
        list->pNextId=2;
        return;
    }
   person p;
   while (read(fd, &p,sizeof(person))==sizeof(person)){
        List* newItem=(List*)malloc(sizeof(List));
        newItem->pList=p;
        newItem->pNext=NULL;
        newItem->pPrev=NULL;
        
        if (list->head == NULL){
            list->head = list->tail=newItem;
        } else {
            List *tmp = list->head;
            while (tmp != NULL && strcmp(tmp->pList.secondName,p.secondName)<0){
                tmp=tmp->pNext;
            }
            if (tmp==NULL){
                list->tail->pNext=newItem;
                newItem->pPrev=list->tail;
                list->tail=newItem;
            } else if(tmp == list->head){
                newItem->pNext=list->head;
                list->head->pPrev=newItem;
                list->head = newItem;
            } else{
                newItem->pNext=tmp;
                newItem->pPrev=tmp->pPrev;
                tmp->pPrev->pNext=newItem;
                tmp->pPrev=newItem;
            }
        }
   }
   close(fd);
}

void prepareList(linkedList *list){
    list->head = NULL;
    list->tail= NULL;
    readContactsFromFile(list);
    if (list->head == NULL){
        list->pNextId=2;
        person prepare[2]={
            {0,"aaa","aaa","aaa","aaa"},
            {1,"ccc","ccc","ccc","ccc"}
        };

        for (int i=0;i<2;i++){
            List *newItem = (List*)malloc(sizeof(List));
            newItem->pList = prepare[i];
            newItem->pNext=NULL;
            newItem->pPrev=NULL;

            if (list->head == NULL){
                list->head=list->tail=newItem;
            } else{
                List *tmp =list->head;
                while (tmp != NULL && strcmp(tmp->pList.secondName, prepare[i].secondName)<0){
                    tmp = tmp->pNext;
                }
                if (tmp==NULL){
                    list->tail->pNext=newItem;
                    newItem->pPrev=list->tail;
                    list->tail=newItem;
                } else if(tmp == list->head){
                    newItem->pNext=list->head;
                    list->head->pPrev=newItem;
                    list->head = newItem;
                } else{
                    newItem->pNext=tmp;
                    newItem->pPrev=tmp->pPrev;
                    tmp->pPrev->pNext=newItem;
                    tmp->pPrev=newItem;
                }
            }
        }
    }
}

int main()
{
    linkedList list;
    int count = 0;
    int action = 0;
    int additional_fields;
    int indexId = 0;
    prepareList(&list);
    char pfirstName[20];
    char plastName[20];
    char pphoneNumb[20]={0};
    char pemailAddr[20]={0};
   // setlocale(LC_ALL, "Russian");
while (1){
    menu();
    scanf("%d", &action);
    switch (action)
    {
        case 1:
        {
            printf("\nEnter first name: \n");
            scanf("%s", pfirstName);
            fseek(stdin, 0, SEEK_END);
            printf("Enter second name: \n");
            scanf("%s", plastName);
            fseek(stdin, 0, SEEK_END);
            addPoint:
            printf("Wanna fill in additional fields? (1-y; 2-n) \n");
            scanf("%d", &additional_fields);
            fseek(stdin, 0, SEEK_END);
            if (additional_fields==1)
            {
                printf("Enter phone number: \n");
                scanf("%s", pphoneNumb);
                fseek(stdin, 0, SEEK_END);
                printf("Enter email: \n");
                scanf("%s", pemailAddr);
                fseek(stdin, 0, SEEK_END);
            }
            else if (additional_fields != 2)
            {
                printf("Not an option\n");
                goto addPoint;
            }
            insertPerson(&list, pfirstName, plastName, pphoneNumb, pemailAddr);
            break;
        }
        case 2:
        {
            printList(&list);
            break;
        }
        case 3:
        {
            printf("Enter person-to-delete ID: ");
            scanf("%d", &indexId);
            fseek(stdin, 0, SEEK_END);
            deletePersonList(indexId, &list);
            break;
        }
        case 4:
        {
            printf("Enter person-to-edit ID: ");
            scanf("%d", &action);
            fseek(stdin, 0, SEEK_END);
            printf("\nEnter first name: \n");
            scanf("%s", pfirstName);
            fseek(stdin, 0, SEEK_END);
            printf("Enter second name: \n");
            scanf("%s", plastName);
            fseek(stdin, 0, SEEK_END);
            editPoint:
            printf("Wanna edit additional fields? (1-y 2-n)");
            scanf("%d", &additional_fields);
            fseek(stdin, 0, SEEK_END);
            if (additional_fields==1)
            {
                printf("\nEnter phone: \n");
                scanf("%s", pphoneNumb);
                fseek(stdin, 0, SEEK_END);
                printf("Enter email: \n");
                scanf("%s", pemailAddr);
                fseek(stdin, 0, SEEK_END);
            }
            else if (additional_fields!=2)
            {
                printf("Not an option\n");
                goto editPoint;
            }
            editPersonList(&list, action, additional_fields, pfirstName, plastName, pphoneNumb, pemailAddr);
            break;
        }
        default:{
            writeContactsToFile(&list);
            deleteList(&list);
            return 0;
        }
    }
}
}