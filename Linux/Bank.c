#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Bank.h"
#include "mystring.h"

extern BANK *B_Head;
extern BANK *FoundBank;
extern char *BANKfile;

void SyncBANK() //"Bank_Details.csv"
{
    BANK *new_node,*temp;
    char *p,DUMMY[20];
    FILE* fp = fopen(BANKfile,"r");
    if(fp == NULL)
    {
        printf("file Load Failed\n");
        exit(0);
    }
    else
    {
        /*** link to list ***/
        if(B_Head == NULL) /** list is empty **/
        {
            char str[100];
            int flag = 0;
            fgets(str,100,fp);
            while(fgets(str,100,fp))
            {
                int i=0;
                new_node=calloc(1,sizeof(BANK));
                for(p = str;p = strtok(p,",");p = NULL)
                {
                    switch(i)
                    {
                        case 0: new_node->username = str_Alloc(p); break;
                        case 1: new_node->bankname = str_Alloc(p); break;
                        case 2: new_node->AccNo = str_Alloc(p); break;
                        case 3: new_node->IFSC = str_Alloc(p); break;
                        case 4: new_node->balance = atoi(p); break;    
                        case 5: new_node->PIN = atoi(p); break;
                        case 6: strcpy(new_node->ID,p); break;
                        case 7: strcpy(new_node->stat,p); break;
                        case 8: new_node->place =  str_Alloc(p);
                                break;
                    }    
                    i++;
                }
                if(B_Head == NULL)
                {
                    B_Head = temp = new_node;
                }
                else
                {
                    temp->next = new_node;
                    temp = temp->next;
                }    
            }
        }
    }
    fclose(fp);
}

void Print_Bank_Details(BANK *h,char ch)
{
    if(h == NULL)
    {
        printf("\n+++++++++++++++++       +++++++++++++++++++\n");
        printf("BANK DETAILS NOT FOUND \n Please TRY AGAIN!!!\n");
        printf("+++++++++++++++++++       +++++++++++++++++++\n\n");
    }
    else if(ch == 1)
    {
        FoundBank = h;
        
        printf("\n                 ~~~~~~~~~~~~~BANK DETAILS ~~~~~~~~~~~~~~\n");
        printf("||--USERNAME--||-BANKNAME-||--ACC.NO--||----IFSC----||---PLACE---||--BALANCE--||--STATUS--||\n");
        printf("|   %-14s %-10s%-10s  %-10s    %-10s    %-10.2f   %-9s|\n",h->username,h->bankname,h->AccNo,h->IFSC,h->place,h->balance,h->stat);
        printf("|------------------------------------------------------------------------------------------|\n");
    }
    else 
    {
        
        printf("\n                 ~~~~~~~~~~~~~BANK DETAILS ~~~~~~~~~~~~~~\n");
        printf("||--USERNAME--||-BANKNAME-||--ACC.NO--||----IFSC----||---PLACE---||--BALANCE--||--STATUS--||  ID\n");
        do
        {
            printf("|   %-14s %-10s%-10s  %-10s    %-10s    %-10.2f   %-9s|  %s\n",h->username,h->bankname,h->AccNo,h->IFSC,h->place,h->balance,h->stat,h->ID);
            h = h->next;
        } while (h);
        printf("|------------------------------------------------------------------------------------------|  \n");        
        
    }
}

void SaveBANK(BANK *h)
{
    if(h == NULL)
    {
        printf("NO ITEMS TO SAVE\n");
    }
    else
    {
        FILE *fp = fopen(BANKfile,"w");
        fprintf(fp,"USERNAME,BANKNAME,ACC.NO,IFSC,BALANCE,PIN,ID,STATUS,PLACE\n");
        while(h)
        {
            fprintf(fp,"%s,%s,%s,%s,%.2f,%d,%s,%s,%s,\n",h->username,h->bankname,h->AccNo,h->IFSC,h->balance,h->PIN,h->ID,h->stat,h->place);
            h = h->next;
        }
        fclose(fp);
    }   
}

BANK* CheckATM(char* ID)
{
    BANK * head = B_Head;
    if(head == NULL)
    {
        printf("NULL ERRROR: \n");
        exit(0);
    }
    while(head && ((strcmp(ID,head->ID)!=0)))
    {
        head = head->next;
    }
    return head;
}

int CheckPass(char *pass)
{
    if(atoi(pass) == FoundBank->PIN) return 1;
    return 0;
}

int DeductAmt(float bill)
{
    if(FoundBank->balance > bill)
    {
        FoundBank->balance -= bill;
        printf("Payment SUCCESS\n");
        return 1;
    }
    printf("Transcation Failed : Insufficient Balance\n");
    return 0;    
}
