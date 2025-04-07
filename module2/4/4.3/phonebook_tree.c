//Поля в дереве отсортированы по фамилии пользователя.
//Вызовы функций addperson в начале тела функции main предназначены для
//упрощения тестирования программы. Для адекватной работы политики ID адресов 
//рекомендуется не вызывать функции addperson с произвольными параметрами.
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>



void menu()
{
    printf("Choose action: \n");
    printf("1.Enter person \n");
    printf("2.Get list of persons \n");
    printf("3.Show tree\n");
    printf("4.Delete person \n");
    printf("5.Edit person \n");
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

typedef struct BST{
    struct person pData;
    struct BST *pLeft;
    struct BST *pRight;
}
BST;

void addperson(BST **node, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr)
{
    if (*node==NULL){
        *node=malloc(sizeof(BST));
        (*node)->pLeft=(*node)->pRight=NULL;
        (*node)->pData.personID=ID;
        (*node)->pData.additionalFields=addFld;
        strcpy( (*node)->pData.firstName, firstName);
        strcpy( (*node)->pData.secondName, secondName);
        if (addFld)
        {
            strcpy( (*node)->pData.phoneNumb, phoneNumb);
            strcpy( (*node)->pData.emailAddr, emailAddr);
        }
        return;
    }
    strcmp(secondName,(*node)->pData.secondName);
    if (strcmp(secondName,(*node)->pData.secondName)>0)
        addperson(&(*node)->pLeft, ID, addFld, firstName, secondName, phoneNumb, emailAddr);
    else 
        addperson(&(*node)->pRight, ID, addFld, firstName, secondName, phoneNumb, emailAddr);
}

BST *deleteNode(BST* t, BST *t0){
    if (t->pLeft != NULL){//идем до упора влево
        t->pLeft = deleteNode(t->pLeft,t0);//праводе поддерево узла замещения здесь
        return t; 
    }
    //сюда попадаем когда нет левого потомка - узел замещения найден
    t0->pData=t->pData;//свапаем данные из узла замещения в удаляемый узел
    BST *temp=t->pRight;//запоминаем правого потомка узла замещения
    free (t);//удаляем узел замещения
    return temp;
    
}

BST *BSTdelete(BST *root, int ID){
    if (!root)
        return root;
    if (ID > root->pData.personID)//идем влево
        root->pLeft=BSTdelete(root->pLeft,ID);
    else if (ID<root->pData.personID)//идем вправо
        root->pRight=BSTdelete(root->pRight,ID);
    else{
        if (!root->pRight && !root->pLeft){//нет потомков
            free (root);
            return NULL;
        }
        if (root->pLeft && !root->pRight){
            BST *tleft=root->pLeft;//запоминаем потомка слева
            free (root);
            return tleft;
        }
        if (!root->pLeft && root->pRight){
            BST *tright=root->pRight;//запоминаем потомка справа
            free (root);
            return tright;
        }//сюда попадаем, если есть оба потомка
        root->pRight=deleteNode(root->pRight,root);//ищем узел замещения
    }
    return root;//для восстановления структуры (при обратном
    //ходе рекурсии дойдет до корня)
}

void BSTprint(BST *node, int space){
    if (node==NULL)
        return;
    space+=5;
    BSTprint(node->pRight,space);
    printf("\n");
    for(int i=5;i<space;i++)
        printf(" ");
    printf("%d\n",node->pData.personID);
    BSTprint(node->pLeft,space);
}

void printList(BST *root){
    if (root){
        printf("ID: %d\n", root->pData.personID);
        printf("First Name: %s\n", root->pData.firstName);
        printf("Last Name: %s\n", root->pData.secondName);
        if (root->pData.additionalFields==1){
            printf("Phone Number: %s\n", root->pData.phoneNumb);
            printf("Email Address: %s\n", root->pData.emailAddr);
        }
        printList(root->pRight);
        printList(root->pLeft);
    }
}

void BSTedit(BST *root, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr){
    if (!root)
        return;
    if (ID > root->pData.personID)//идем влево
        BSTedit(root->pLeft, ID, addFld, firstName, secondName, phoneNumb, emailAddr);
    else 
        if (ID<root->pData.personID)//идем вправо
            BSTedit(root->pRight, ID, addFld, firstName, secondName, phoneNumb, emailAddr);
    else{
        strcpy(root->pData.firstName, firstName);
        strcpy(root->pData.secondName, secondName);
        if (addFld){
            strcpy(root->pData.phoneNumb, phoneNumb);
            strcpy(root->pData.emailAddr, emailAddr);
        } 
        return;
    }
}

int main()
{
    BST *root=NULL;
    int count = 0;
    int action = 0;
    int indexId = 0;
    char pfirstName[20];
    char plastName[20];
    char pphoneNumb[20];
    char pemailAddr[20];
    //setlocale(LC_ALL, "Russian");
    addperson(&root, 10, 1, "Name10",  "j", "666", "0@gmail.com");
    addperson(&root, 5, 1, "Name51",  "e", "666", "0@gmail.com");
    addperson(&root, 20, 1, "Name12",  "t", "666", "0@gmail.com");
    addperson(&root, 16, 1, "Name41",  "p", "666", "0@gmail.com");
    addperson(&root, 3, 1, "Name33",  "c", "666", "0@gmail.com");
    addperson(&root, 9, 1, "Name88",  "i", "666", "0@gmail.com");
    addperson(&root, 11, 1, "Name99",  "k", "666", "0@gmail.com");
    addperson(&root, 14, 1, "Name66",  "n", "666", "0@gmail.com");
    addperson(&root, 7, 1, "Name22",  "g", "666", "0@gmail.com");
    addperson(&root, 8, 1,"Name19",  "h", "666", "0@gmail.com");
   
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
            addperson(&root, count, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            count++;
            break;
        }
        case 2:
        {
            printList(root);
            break;
        }
        case 3:
        {
            BSTprint(root,0);
            break;
        }
        case 4:
        {
            printf("Enter person-to-delete ID: ");
            scanf("%d", &indexId);
            fseek(stdin, 0, SEEK_END);
            BSTdelete(root,indexId);
            break;
        }
        case 5:
        {
            printf("Enter person-to-edit ID: ");
            scanf("%d", &action);
            fseek(stdin, 0, SEEK_END);
            printf("\nEnter first name: \n");
            scanf("%s", &pfirstName);
            printf("Enter second name: \n");
            scanf("%s", &plastName);
            editPoint2:
            printf("Wanna edit additional fields? (1-y 0-n)");
            scanf("%d", &indexId);
            if (indexId==1)
            {
                printf("\nEnter phone: \n");
                scanf("%s", &pphoneNumb);
                printf("Enter email: \n");
                scanf("%s", &pemailAddr);
            }
            else if (indexId!=0)
            {
                printf("Not an option\n");
                goto editPoint2;
            }
            BSTedit(root, action, indexId, pfirstName, plastName, pphoneNumb, pemailAddr);
            break;
        }
        default:{
            return 0;
        }
    }
}
}