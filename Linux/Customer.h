#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_

typedef struct user_cart
{
    char *name;
    char ID[10];
    float price;
    float quantity;
    struct user_cart *next;
}CART;

CART* Find_item_C(CART* head,char *str);
void addtocart(CART* new_node,char *str);
void SaveCART(CART* head);
float TotalBill_C(CART *head);
void Print_C(CART* head);
CART* DelFirst_C(CART *head);
CART* Delete_C(CART* head,char *str); 
CART* Delete_C_ALL(CART *head);
CART* CustomerMenu(CART* head, int choice);
#endif
