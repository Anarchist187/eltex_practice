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