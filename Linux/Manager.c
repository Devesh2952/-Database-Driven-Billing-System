#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Manager.h"
#include "mystring.h"
#include "unistd.h"

extern LIST* M_Head;
extern char *LISTfile,MCARD[10];

LIST* Find_item(char* str)
{
    LIST *head = M_Head;
    while(head && ((strcmp(str,head->name)!=0) && (strcmp(str,head->ID)!=0)))
    {
        head = head->next;
    }
    return head;
}

void Print(LIST* head)
{    
    if(head == NULL)
    {
        printf("\n+++++++++++++       ++++++++++++\n");
        printf("STOCK EMPTY : No Items in stock\nPlease Refill/Add Items\n");
        printf("+++++++++++++       ++++++++++++\n\n");
    }
    else
    {
        printf("\n       ########## SHOPPING LIST ##########\n");
        printf("||---ITEMS---||--CARD-NO--||--PRICE--||--QUANTITY--||\n");
        do
        {
            printf("|   %-10s   %-10s %7.2f        %-7.2f  |\n",head->name,head->ID,head->price,head->quantity);
            head = head->next;
        } while (head);
        printf("|---------------------------------------------------|\n\n");        
    }
}

void SaveLIST(LIST* head)
{
    if(head == NULL)
    {
        printf("NO ITEMS TO SAVE\n");
    }
    else
    {
        FILE *fp = fopen(LISTfile,"w");
        fprintf(fp,"ITEMS,CARD-NUMBER,PRICE,QUANTITY\n");
        while(head)
        {
            fprintf(fp,"%s,%s,%.2f,%.2f\n",head->name,head->ID,head->price,head->quantity);
            head = head->next;
        }
        fclose(fp);
    }   
}

LIST *SyncLIST()
{
    LIST *new_node=NULL,*head=NULL,*temp;
    char *p;
    FILE *fp = fopen(LISTfile,"r");
    if(fp == NULL)
    {
        printf("file Load Failed\n");
        exit(0);
    }
    else
    {
        /*** link to list ***/
        if(M_Head == NULL) /** list is empty **/
        {
            char str[100];
            int flag = 0;
            fgets(str,100,fp);
            while(fgets(str,100,fp))
            {
                int i=0;
                new_node=calloc(1,sizeof(LIST));
                for(p = str;p = strtok(p,",");p = NULL)
                {
                    if(i==0) new_node->name = str_Alloc(p);
                    if(i==1) strcpy(new_node->ID,p); 
                    if(i==2) new_node->price = atof(p);
                    if(i==3) new_node->quantity = atof(p);
                    i++;
                }
                if(head == NULL)
                {
                    head = temp = new_node;
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
    return head;
}

LIST* DelFirst(LIST *head)
{
    LIST* temp;
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

LIST* Delete(LIST* head) // To Delete a NODE(ITEM), BASED ON RFID NUMBER
{
    LIST* temp = head,*dummy,*prev =head;
    char *str;
    printf("Enter Item Name or Card NO to DELETE : ");
    str = getString();
    if(head == NULL)
    {
        printf("EMPTY NODE(ITEM)");
    }
    else if(strcmp(head->name,str)==0 || strcmp(head->ID,str)==0) 
    {
            head = DelFirst(head);     
    }
    else
    {   
        prev = temp;
        temp = temp->next;
        while(temp)
        {
            if((strcmp(temp->name,str)==0) || (strcmp(temp->ID,str)==0)) 
            {
                    if(temp->next == NULL)
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
                break;
            }
            prev = temp;    
            temp = temp->next;
        }
    }
    return head;
}

void addtolist(LIST* new_node)
{
    printf("Enter Card No: ");
    scanf("%s", new_node->ID);
    printf("Enter ITEM NAME: ");
    new_node->name = getString();
    printf("Enter Price: ");
    scanf("%f", &new_node->price);
    printf("Enter Quantity: ");
    scanf("%f", &new_node->quantity);
}

LIST* AddLast(LIST *head,char ch)
{
    LIST *new_node=NULL,*temp=NULL;
    new_node=calloc(1,sizeof(LIST));
	if(new_node==NULL)
	{
		printf("memory allocation failed\n");
	}
    else
    {
        if(ch == 'M')
            addtolist(new_node);
        /*** link to list ***/
        if(head == NULL) /** list is empty **/
        {
            M_Head = head = new_node;   
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
    if(ch == 'M')
        return head;
    else if(ch == 'R')
        return new_node;
}

void UpdateDB()
{
	int choice;
	char str[20];
	LIST *Updt_node;
	while(1)
	{	
        printf("\n|||| TO EXIT type exit and Enter ||||\n");
        printf("\nEnter Name or ID to update : ");
		scanf("%s",str);
        if(!strcmp(str,"exit")) break;
		Updt_node = Find_item(str);	
		if(Updt_node)
		{	
			printf("\n+++++++++++ Current Data of ITEM +++++++++++\n\n");
         		printf("||---ITEMS---||--CARD-NO--||--PRICE--||--QUANTITY--||\n");
			printf("|   %-10s   %-10s %7.2f        %-7.2f  |\n",Updt_node->name,Updt_node->ID,Updt_node->price,Updt_node->quantity);
                	printf("|---------------------------------------------------|\n");

			printf("\n---- MENU -----\n");
			printf("1. Update Name\n");
			printf("2. Update Price\n");
			printf("3. Update Quantity\n");
			printf("4. Update ID\n");
			printf("5. Update ALL\n");
			printf("6. Exit from Update MENU\n");
			printf("\n Enter Choice : ");
			scanf("%d",&choice);
			switch(choice)
			{
				case 1: free(Updt_node->name);
					printf("Enter New ITEM NAME : ");
					Updt_node->name = getString();
					printf("****** Name Updated Successfully ******\n");
					break;
				case 2: printf("Enter New Price : ");
					scanf("%f",&Updt_node->price);
					printf("****** Price Updated Successfully ******\n");
					break;
				case 3: printf("Enter New Quantity : ");
					scanf("%f",&Updt_node->quantity);
					printf("****** Quantity Updated Successfully ******\n");
					break;
				case 4: 
					printf("Enter New ID : ");
					scanf("%s",Updt_node->ID);
					printf("****** ID Updated Successfully ******\n");
					break;
				case 5: 
					free(Updt_node->name);
					printf("Enter New ITEM NAME : ");
					Updt_node->name = getString();
					printf("Enter New ID : ");
					scanf("%s",Updt_node->ID);
					printf("Enter New Price : ");
					scanf("%f",&Updt_node->price);
					printf("Enter New Quantity : ");
					scanf("%f",&Updt_node->quantity);
					printf("\n****** Data Updated Successfully ******\n");
					break;
				case 6: 
					return;
				default:
					printf("\nInvalid Choice !\n\n");		
			}
		}
		else
		{
			printf("\nError: Entered Item ID/Name Not FOUND\n\n");
		}
	}
}

void Find_Delete(LIST *head,char *str) // To Delete a NODE(ITEM), BASED ON RFID NUMBER
{
    LIST* temp = head,*dummy,*prev =head;
    if(head == NULL)
    {
        printf("EMPTY NODE(ITEM)");
    }
    else if(strcmp(head->name,str)==0 || strcmp(head->ID,str)==0) 
    {
            head = DelFirst(head);     
    }
    else
    {   
        prev = temp;
        temp = temp->next;
        while(temp)
        {
            if((strcmp(temp->name,str)==0) || (strcmp(temp->ID,str)==0)) 
            {              
                    if(temp->next == NULL)
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
                break;
            }
            prev = temp;    
            temp = temp->next;
        }
    }
    M_Head = head;
}

LIST * ManagerMenu(LIST *head)
{
    int choice;
    while (1)
    {   
        system("clear");
        Print(head);

        printf("\n-------------MENU------------\n");
        printf("1. Add ITEM\n");
        printf("2. View SHOP LIST\n");
        printf("3. Sync From DATABASE CSV FILE \n");
        printf("4. Delete ITEM\n");
	    printf("5. Update DATABASE\n");
        printf("6. UPDATE TO DATABASE and Exit\n");
        printf("7. Exit from MANAGER MODE (UPDATES Added Temporarily in App)\n");

        printf("Enter choice: ");
        scanf("%d", &choice);
        printf("\n");
        switch (choice)
        {
            case 1:
                head = AddLast(head,'M');
                break;
            case 2:
                Print(head);
                break;
            case 3:
                head = SyncLIST();
                break;         
            case 4:
                head = Delete(head);
                break;
	        case 5:
		        UpdateDB();
		        break;
            case 6:
                SaveLIST(head);
                printf("Exiting... DATABASE UPDATED\n");
                sleep(1);
                return head;
            case 7:
                return head;
            case 235:
mupdate:        puts("MANAGER CARD UPDATE\nMENU =  1. UPDATE  2. VIEW CARD NO  3. DISCARD");
                scanf("%d",&choice);
                switch(choice)
                {
                    case 1: printf("Enter new Manager card no - 8 digits\n");
                            scanf("%s",MCARD);
                            puts("MANAGER CARD UPDATED");
                            break;
                    case 2: printf("MCARD no : %s\n",MCARD); break;
                    case 3: printf("NO CHANGES DONE\n"); break;
                    default: puts("INVALID CHOICE"); goto mupdate;
                }
                               
            default:
                printf("Invalid choice. Please try again.\n");
        }
        sleep(3);
    }   
}
