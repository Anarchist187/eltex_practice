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
p[20];

void addperson(int cnt, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr)
{
    p[cnt].personID = cnt;
    p[cnt].additionalFields = addFld;
    strcpy(p[cnt].firstName, firstName);
    strcpy(p[cnt].secondName, secondName);
    if (addFld)
    {
       strcpy(p[cnt].phoneNumb, phoneNumb);
       strcpy(p[cnt].emailAddr, emailAddr);
    }
}

void deleteperson(int delId, int cnt)
{
    for (int i = delId; i < cnt - 1; i++)
    {
        p[i] = p[i + 1];
        p[i].personID = i;
    }
}

void editperson(int editId, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr)
{
    strcpy(p[editId].firstName, firstName);
    strcpy(p[editId].secondName, secondName);
    if (addFld)
    {
        strcpy(p[editId].phoneNumb, phoneNumb);
        strcpy(p[editId].emailAddr, emailAddr);
    }
}

int main()
{
    int count = 0;
    int action = 0;
    int indexId = 0;
    char *pfirstName;
    char *plastName;
    char *pphoneNumb;
    char *pemailAddr;
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
            printf("Enter second name: \n");
            scanf("%s", &plastName);
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
            addperson(count, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            count++;
            break;
        }
        case 2:
        {
            for (int i = 0; i < count; i++)
            {
                printf("ID: %d\n", p[i].personID);
                printf("First Name: %s\n", p[i].firstName);
                printf("Last Name: %s\n", p[i].secondName);
                if (p[i].additionalFields==1)
                {
                    printf("Phone Number: %s\n", p[i].phoneNumb);
                    printf("Email Address: %s\n", p[i].emailAddr);
                }
            }
            break;
        }
        case 3:
        {
            printf("Enter person-to-delete ID: ");
            scanf("%d", &indexId);
            fseek(stdin, 0, SEEK_END);
            deleteperson(indexId, count);
            count--;
            break;
        }
        case 4:
        {
            printf("Enter person-to-edit ID: ");
            scanf("%d", &action);
            fseek(stdin, 0, SEEK_END);
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
            else if (indexId==2)
                break;
            else
            {
                printf("Not an option\n");
                goto editPoint;
            }
            editperson(action, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            break;
        }
        default:
            return 0;
    }
}
}