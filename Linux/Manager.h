#ifndef _MANAGER_H_
#define _MANAGER_H_

typedef struct Items_list
{
    char *name;
    char ID[10];
    float price;
    float quantity;
    struct Items_list *next;
}LIST;

LIST* Find_item(char* str);
void Print(LIST* head);
void SaveLIST(LIST* head);
LIST *SyncLIST();
LIST* DelFirst(LIST *head);
LIST* Delete(LIST* head); 
void addtolist(LIST* new_node);
LIST* AddLast(LIST *head,char ch);
void UpdateDB();
void Find_Delete(LIST *head,char *str);
LIST * ManagerMenu(LIST *head);
#endif
