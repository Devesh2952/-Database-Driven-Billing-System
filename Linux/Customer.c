//#define DEBUG
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "Customer.h"
#include "Manager.h"
#include "Bank.h"
#include "uart.h"
#include "mystring.h"

extern LIST* M_Head;
extern CART* C_Head;
extern float bill;
extern BANK *B_Head;
extern char BUFF[30],ch,ID[20],PIN[10],*CARTfile;
extern int fd,chances;

void write_str_serial(int fd,char* str);

CART* Find_item_C(CART* head,char *str)
{
    while(head && ((strcmp(str,head->name)!=0) && (strcmp(str,head->ID)!=0)))
    {
        head = head->next;
    }
    return head;
}

void addtocart(CART* new_node,char *str)
{
    LIST * Found_Item;
    Found_Item = Find_item(str);
    if(Found_Item)
    {
        new_node->name = Found_Item->name;
        new_node->price = Found_Item->price;
        strcpy(new_node->ID,Found_Item->ID);
        new_node->quantity++;
        Found_Item->quantity--;
        printf("%s Item Added to CART Successfully\n",Found_Item->name);
        sleep(1);
        sprintf(BUFF,"1%s",Found_Item->name);
        #ifndef DEBUG
        write_str_serial(fd,BUFF);
        #else
        puts(BUFF);
        #endif
        sleep(1);
        sprintf(BUFF,"STOCK: %.0f CART: %.0f",Found_Item->quantity,new_node->quantity);
        printf("%s\n",BUFF);
        #ifndef DEBUG
        write_str_serial(fd,BUFF);
        #else
        puts(BUFF);
        #endif
        if(Found_Item->quantity == 0)
        {
            Find_Delete(M_Head,Found_Item->name);
        }
    }
    else
    {
        printf("Item Not Available in LIST\n");
        sleep(1);
        sprintf(BUFF,"0ITEM");
        #ifndef DEBUG
        write_str_serial(fd,BUFF);
        #else
        puts(BUFF);
        #endif
    }
}

void SaveCART(CART* head)
{
    if(head == NULL)
    {
        printf("NO ITEMS TO SAVE\n");
    }
    else
    {
        FILE *fp = fopen(CARTfile,"w");
        fprintf(fp,"ITEMS,PRICE,QUANTITY\n");
        while(head)
        {
            fprintf(fp,"%s,%.2f,%.2f\n",head->name,head->price,head->quantity);
            head = head->next;
        }
        fprintf(fp,"-----------------------\nTOTAL BILL = %.2f\n-----------------------",bill);
        fclose(fp);
    }   
}

float TotalBill_C(CART *head)
{
    bill =0;
    if(head == NULL)
    {
        printf("EMPTY NODE\n");
    }
    else
    {
        while(head)
        {
            bill = bill + ((head->price)*(head->quantity)); 
            head = head->next;
        }
    }
    return bill;
}

void Print_C(CART* head) //Prints Cart Items in TABLE FORMAT
{    
    if(head == NULL)
    {
        printf("\n+++++++++++++       ++++++++++++\n");
        printf("CART EMPTY : No Items in Cart\n         Please Add Items\n");
        printf("+++++++++++++       ++++++++++++\n\n");
    }
    else
    {
        printf("\n            ********* USER CART *********\n");
        printf("||---ITEMS---||--PRICE--||--QUANTITY--||---VALUE---||\n");
        do
        {
            printf("|   %-10s %7.2f        %-7.2f      %-9.2f|\n",head->name,head->price,head->quantity,(head->price*head->quantity));
            head = head->next;
        } while (head);
        printf("|---------------------------------------------------|\n");
        printf("|                         TOTAL BILL   -   %-9.2f|\n",TotalBill_C(C_Head));
        printf("|---------------------------------------------------|\n\n");        
    }
}

CART* DelFirst_C(CART *head)
{
    CART* temp;
    if(head == NULL)
    {
        printf("EMPTY NODE\n");
    }
    else
    {
        temp = head;
        head = head->next;
        free(temp);
        temp = NULL;
    }
    return head;
}

CART* Delete_C(CART* head,char *str) // To Delete a NODE(ITEM), BASED ON RFID NUMBER
{
    CART* temp = head,*dummy,*prev =head;
    LIST *Returnitem = Find_item(str);
    int flag;
    char *name,BUFF2[30];
    if(head == NULL)
    {
        flag = -1;
        printf("ERROR: NO ITEMS IN CART : CART EMPTY : PLEASE ADD ITEMS\n");
        sprintf(BUFF,"X");
    }
    else
    {   
        prev = temp;
        while(temp)
        {
            if((strcmp(temp->name,str)==0) || (strcmp(temp->ID,str)==0)) 
            {
                flag = 1;
                if(Returnitem == NULL)
                {
                    Returnitem = AddLast(M_Head,'R');
                    strcpy(Returnitem->ID,temp->ID);
                    Returnitem->name = temp->name;
                    Returnitem->price = temp->price;
                }
                temp->quantity--;
                Returnitem->quantity++;
                printf("%s : DELETE : SUCCESSFULLY REDUCED 1 (QUANTITY)\n",temp->name);
                sprintf(BUFF,"1%s",temp->name);
                sprintf(BUFF2,"STOCK: %.0f CART: %.0f",Returnitem->quantity,temp->quantity);
                printf("%s\n",BUFF2);
                if(temp->quantity==0)
                {
                    flag = 2;
                    printf("%s : DELETE : REMOVED FROM CART\n",temp->name); 
                    sprintf(BUFF,"2%s",temp->name);
                    sprintf(BUFF2,"STOCK: %.0f CART: %.0f",Returnitem->quantity,temp->quantity);  
                    if(prev == temp)
                    {
                        head = DelFirst_C(head);     
                    }                
                    else if(temp->next == NULL)
                    {
                        dummy = temp;
                        prev->next = NULL;
                        free(dummy); 
                    }
                    else
                    {
                        dummy = temp;
                        prev->next = temp->next;
                        free(dummy);
                    }
                }
                break;
            }
            prev = temp;    
            temp = temp->next;
        }
        if(temp == NULL)
        {
            flag = 0;
            printf("ERROR : ITEM IS NOT IN YOUR CART : PLEASE ADD ITEMS\n");
            sprintf(BUFF,"0");
        }
    }
    sleep(1);
    #ifndef DEBUG
        write_str_serial(fd,BUFF);
        #else
        puts(BUFF);
        #endif
    sleep(1); 
    switch(flag)
    {
        case 1: 
        #ifndef DEBUG
        write_str_serial(fd,BUFF2);
        #else
        puts(BUFF2);
        #endif 
        break;          
        case 2: 
        #ifndef DEBUG
        write_str_serial(fd,BUFF2);
        #else
        puts(BUFF2);
        #endif 
        break;
    }    
    return head;
}

CART* AddLast_C(CART *head,char *str)
{
    CART *new_node=NULL,*temp=NULL;
    LIST *Updt_List_Q; 

    temp = Find_item_C(head,str);
    Updt_List_Q = Find_item(str);
    if(Updt_List_Q == NULL)
    {
        printf("*** %s : is Not in STOCK ***\n",str);
        sleep(1);
        sprintf(BUFF,"0ITEM");
        #ifndef DEBUG
        write_str_serial(fd,BUFF);
        #else
        puts(BUFF);
        #endif
        goto Addexit;
    }    
    if(temp)
    {
        if(Updt_List_Q->quantity > 0)
        {    
            temp->quantity++;
            Updt_List_Q->quantity--;
            printf("%s IS AVAILABLE IN CART .... UPDATING QUANTITY\n",temp->name);
            sleep(1);
            sprintf(BUFF,"2%s",temp->name);
            #ifndef DEBUG
            write_str_serial(fd,BUFF);
            #else
            puts(BUFF);
            #endif
            sleep(1);
            sprintf(BUFF,"STOCK: %.0f CART: %.0f",Updt_List_Q->quantity,temp->quantity);
            printf("%s\n",BUFF);
            #ifndef DEBUG
            write_str_serial(fd,BUFF);
            #else
            puts(BUFF);
            #endif
            if(Updt_List_Q->quantity == 0)
            {
                Find_Delete(M_Head,Updt_List_Q->name);
            }
        }           
    }
    else
    {
        new_node=calloc(1,sizeof(CART));
        if(new_node==NULL)
        {
            printf("memory allocation failed\n");
        }
        else
        {
            addtocart(new_node,str);
            /*** link to list ***/
            if(head == NULL) /** cart list is empty **/
            {
                head = new_node;   
            }
            else
            {
                temp=head;
                while(temp->next) // traversing upto last node
                {
                    temp=temp->next; // jumpling to last node
                }
                temp->next=new_node;
            }
        }

    }
Addexit:        
    return head;
}

CART* Delete_C_ALL(CART *head)
{
    CART *temp;
    while(head)
    {
        temp = head;
        head = head->next;
        //free(temp->name);
        free(temp);
    }
    return NULL;
}

void Ret_to_LIST()
{
    LIST *mfound;
    CART *chead = C_Head;
    while(chead)
    {
        mfound = Find_item(chead->name);
        if(mfound == NULL){
            mfound = AddLast(M_Head,'R');
            strcpy(mfound->ID,chead->ID);
            mfound->name = chead->name;
            mfound->price = chead->price;
        }
        mfound->quantity+=chead->quantity;
        chead = chead->next;
    }
}

CART* CustomerMenu(CART* head, int choice)
{
    int k=0;
    BANK *FoundBank;
    char rx;
    char *rxstr;
/*        printf("\n-------------MENU------------\n");
        printf("1. AddLast\n");
        printf("2. View SHOP CART\n");
        printf("3. Save and Exit\n");
        printf("4. Delete ITEM\n");
        printf("5. BILLING \n");
        printf("Enter choice: ");
        scanf("%d", &choice); */
        printf("\n");
        switch (choice)
        {
            case 1:
                head = AddLast_C(head,ID);
                break;
            case 2:
                Print_C(head);
                break;
            case 3:
                SaveCART(head);
                printf("Exiting...\n");
                break;
            case 4:
                head = Delete_C(head,ID);
                break;
            case 5:
                FoundBank = CheckATM(ID);
                if(FoundBank == NULL) 
                {   
                    printf("ERROR : Invalid ATM CARD\n");
                    sleep(2);
                    #ifndef DEBUG
                    serialPutStr(fd,"@F$");
                    #else
                    puts("@F$");
                    #endif
                    break;
                }    
                Print_Bank_Details(FoundBank,1);
                if(!strcmp(FoundBank->stat,"BLOCKED"))
                {
                    printf("CARD BLOCKED\n *** PLEASE SWIPE ANOTHER CARD ***\n");
                    #ifndef DEBUG
                    serialPutStr(fd,"@B$");
                    #else
                    puts("@B$");
                    #endif
                    sleep(2);
                    break;
                }
try:                printf("Enter C: Confirm E: Exit\n");
                __fpurge(stdin);
                scanf("%c",&ch);
                switch(ch)
                {
                    case 'C':    
                    #ifndef DEBUG
                    serialPutStr(fd,"@C$");
                    #else
                    puts("@C$");
                    #endif
                                usleep(10);
                                printf("Enter Password in KEYPAD...\n");
                                #ifndef DEBUG
                                serialFlush(fd);
                                while((rx = serialGetchar(fd))!= '$')
                                {
                                    if(rx == '@') k=0;
                                    else PIN[k++] = rx;
                                }
                                PIN[k] = '\0';
                                #else
                                scanf("%s",PIN);
                                #endif
                                
                                k=0; 
                                if(PIN[0] == 'X')
                                {
                                    printf("TRANSACTION CANCELLED... GOING BACK\n");
                                    break;
                                }  
                                //printf("PASSWORD received : %s",PIN);
                                if(CheckPass(PIN))
                                {
                                    #ifndef DEBUG
                                    serialFlush(fd);
                                    #endif
                                    if(DeductAmt(bill))
                                    {
                                        printf("Transaction Success\n");
                                        sleep(1);
                                        #ifndef DEBUG
                                        serialPutStr(fd,"@S$");
                                        #else
                                        puts("@S$");
                                        #endif
                                        SaveBANK(B_Head);
                                        SaveLIST(M_Head);
                                        SaveCART(head);
                                        head = Delete_C_ALL(head);
                                    }
                                    else
                                    {
                                        printf("INSUFFICIENT BALANCE\n");
                                        sleep(1);
                                        #ifndef DEBUG
                                        serialPutStr(fd,"@F$");
                                        #else
                                        puts("@F$");
                                        #endif
                                        Ret_to_LIST();
                                        head = Delete_C_ALL(head);
                                    }
                                    #ifndef DEBUG
                                    serialFlush(fd);
                                    #endif
                                }
                                else if(chances<=3)
                                {
                                    chances++;
                                    printf("Transaction Failed\n");
                                    printf("Wrong PASSWORD ENTERED\n");         //    1st   2nd  3rd  4th   5th Attempt
                                    printf("%d chances Remaining\n",(4-chances));   //    3      2   1    0     blocked
                                    if((4-chances)==0) printf("LAST CHANCE\n");
                                    sleep(1);                                //    1      2   3    4
                                    #ifndef DEBUG
                                    serialPutStr(fd,"@F$");
                                    #else
                                    puts("@F$");
                                    #endif
                                }
                                else
                                {
                                    printf("ATM BLOCKED\n");
                                    printf("TRY AGAIN AFTER 24HOURS\n");
                                    sleep(1);
                                    #ifndef DEBUG
                                    serialPutStr(fd,"@F$");
                                    #else
                                    puts("@F$");
                                    #endif
                                    strcpy(FoundBank->stat,"BLOCKED");
                                    SaveBANK(B_Head);
                                    Ret_to_LIST();
                                    head = Delete_C_ALL(head);
                                    chances = 0;
                                    break;
                                } 
                                break;

                    case 'E':   sleep(1);
                                #ifndef DEBUG
                                        serialPutStr(fd,"@E$");
                                        #else
                                        puts("@E$");
                                        #endif
                                printf("TRANSACTION CANCELLED... EXITING\n");
                                printf("CART DELETING...\n");
                                bill =0;
                                Ret_to_LIST();
                                head = Delete_C_ALL(head); 
                                break;
                    default:   printf("Invalid OPTION.. TRY AGAIN\n");
                                goto try;
                }
                break;               
            default:
                printf("Invalid choice. Please try again.\n");
        } 
    return head;
}
