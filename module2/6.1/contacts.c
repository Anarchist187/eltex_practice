#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <contacts.h>

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