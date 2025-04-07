typedef struct person
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
void menu();
struct List *insertPerson(struct List* head, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr);
struct List* deleteList(struct List* head);
struct List* deletePersonList(int ID,struct List* head);
struct List* rearrangeID(struct List* head, int count);
void printList (struct List *head);
struct List* editPersonList(struct List* head, int ID, int addFld, char* firstName, char* secondName, char* phoneNumb, char* emailAddr);
