#define LAB
//#define WIN
//#define DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "uart.h"
#include <errno.h>
#include "Bank.h"
#include "Manager.h"
#include "Customer.h"

int fd,chances;
char ch,ID[20],BILL_AMOUNT[20],MCARD[10] = "00325094",PIN[10],BUFF[30];

char *LISTfile = "ITEMS_LIST_L.csv",*CARTfile = "User_Cart_L.csv",*BANKfile = "Bank_Details_L.csv"; // DATABASE FILES NAMES

LIST* M_Head;
CART* C_Head;
float bill =0;

BANK *B_Head;
BANK *FoundBank;
#ifndef DEBUG
                                //Reads from serial COM PORT // Only collects $ terminated string
char read_str_serial(int fd)    //Returns first char as choice .. then upto $ LOADS characters into ID array
{
    int i=0;
    char rx;
    char choice = serialGetchar(fd);
    while((rx=serialGetchar(fd)) != '$')    //STRING RECEIVE TYPE - M12345678$
    {
        ID[i++] = rx;
    }
    ID[i] = '\0';
    i=0;
    return choice;
}

void write_str_serial(int fd,char* str)   //STRING SEND TYPE - @1234$ //Writes to Serial COM PORT
{
    serialPutchar(fd,'@');    //THIS FUNC WILL Add SOT as '@' // EOT as '$' 
    while(*str)
    {
        serialPutchar(fd,*str);
        str++;
    }
    serialPutchar(fd,'$');
}
#endif

/////////////////////  MAIN ///////////////////////////
int main(int argc, char **argv)
{
  if(argc == 2)
  {
      strcpy(MCARD,argv[1]);
      puts("MASTER CARD NO UPDATED SUCCESSFULLY");
  }
#ifndef DEBUG
    fd = Init_Serial();
#endif
    char choice;
    M_Head = SyncLIST(LISTfile);
    SyncBANK("Bank_Details.csv");

    /*      DEBUG ID CARD NUMBERS & FORMATS     */
//                          0123456789      0123456789
//          ITEM 1          @96374568$      C96374568$      D96374568$
//          ITEM 2          @17492084$      C17492084$      D17492084$
//          ITEM 3          @05106529$      C05106529$      D05106529$
//          MASTER          @01267468$      M01267468$      
//          ATMCRD          @26768965$      B26768965$  
//                          0123456789      0123456789

    while(1)
    {
        #ifndef DEBUG
        serialFlush(fd);
        #endif
        system("clear");
    //  Print_Bank_Details(B_Head,0);
        Print(M_Head);      //Prints TABLE OF ITEMS LIST FROM DATABASE
        Print_C(C_Head);    //PRINTS TABLE OF ITEMS FROM USER CART
        #ifndef DEBUG
            choice = read_str_serial(fd);   // CHARACTERS RECEIVED AS SIGNAL FROM HARDWARE BOARD
        #else
            printf("Enter Choice : ");
            __fpurge(stdin);
            scanf("%c",&ch);
            printf("Enter ID : "); scanf("%s",ID);
        #endif
        if(!strcmp(ID,MCARD) && choice == 'C') //only check master card in entry mode
        {
            choice = 'M'; // if matches update to master card symbol
        }
        switch (choice)
        {
            case 'M':   // M01267468$  @01267468$       // MANAGER 
                //printf ("MANAGER : recvd : %s\n",ID);
                sleep(1);
                #ifndef DEBUG
                serialPutStr(fd,"@M$"); //sends start manager mode signal to hardware
                #else
                puts("M");
                #endif
                M_Head = ManagerMenu(M_Head);
                #ifndef DEBUG
                serialPutStr(fd,"@X$"); //sends exit manager mode signal to hardware
                #else
                puts("X");
                #endif
                break;

            case 'C':  //C1: C96374568$   C2: C17492084$   C3: C05106529$  // ADD ITEMs
                printf ("CUSTOMER : recvd : %s\n",ID);
                C_Head = CustomerMenu(C_Head, 1);  // 1 for ADD ITEM to CART LINKED LIST
                break;

            case 'D':   // DELETE ITEM
                printf ("DELETE : recvd : %s\n",ID); 
                C_Head = CustomerMenu(C_Head, 4);   // 4 for DELETE ITEM from CART LINKED LIST
                if(C_Head == NULL) bill = 0;
                break;

            case 'B':   // PAYMENT BY CARD
                printf ("ATM ID : recvd : %s\n",ID);
                C_Head = CustomerMenu(C_Head, 5);   // 5 for CART ITEMS BILLING 
                break;

            case 'A':   // TRANSMITS BILL AS STRING TO COM PORT (HARDWARE) 
                usleep(10000);
                if(bill == 0)
                {
                    printf("NO ITEMS IN CART .. PLEASE ADD ITEMS\n");
                }
                //printf("Total Bill : %.2f\n",bill);
                sprintf(BILL_AMOUNT,"%.2f",bill);
                #ifndef DEBUG
                usleep(10000);
                //printf("Sending : %s\n",BILL_AMOUNT);
                serialFlush(fd);
                sleep(1);
                write_str_serial(fd,BILL_AMOUNT);  /*   for(int i=0;BILL_AMOUNT[i];i++)
                                                        {
                                                            serialPutchar(fd,BILL_AMOUNT[i]);
                                                            usleep(500);
                                                        } */
                usleep(10000);
                #else
                puts(BILL_AMOUNT);
                #endif
                break;

            case 'Q':
                if (ID[0] == '1')
                {
                    printf("TRANSACTION SUCCESS : PAID BY CASH\n");
                    SaveCART(C_Head);
                    C_Head = Delete_C_ALL(C_Head);
                    SaveLIST(M_Head);
                }
                else if(ID[0] == '0')
                {
                    printf("CASH TRANSACTION FAILED :\n\n*******  RETRY ******\n\n");
                }
                else if(ID[0] == 'X')
                {
                    printf("CASH TRANSACTION CANCELLED\n");
                }
                break;
        }
    if(choice == 'A' || choice == 'D') usleep(100000);
    else {
        sleep(2);
    }    
    }
}

int Init_Serial()  // INITIALISE SERIAL PORT 
{
	puts("Opening serial port\n");
#ifdef LAB
	if ((fd = serialOpen ("/dev/ttyUSB0",9600)) < 0)    // OPENS THIS DEVICE WHEN PORT IS CONNECTED TO USB PORT
#else
    if ((fd = serialOpen ("/dev/ttyS3",9600)) < 0)      // OPENS THIS DEVICE WHEN PORT IS CONNECTED TO DIRECT COM PORT
#endif        
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		return 1 ;
	}

	puts("serial port is opened\n");

	usleep(1000000);
    return fd;
}

