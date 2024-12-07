#include "delay.h" 
#include "kpm.h" 
#include "kpm_defines.h" 
#include "lcd.h" 
#include "lcd_defines.h" 
#include "types.h" 
#include "uart.h" 
#include "uart_defines.h"
#include "interrupt_defines.h"
#include "interrupt.h"
#include "UART1.h"
#include "myfuncs.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>

//#define PRO
#define DEBUG

int i,t,pass=0,chances =0,Amt,backflag =0;
char key,uflag0 = 0,uflag1 = 0,entryflag=0,exitflag =0,delflag=0,dummy,ch,flag=0;
char ID[30]  = "HELLO",PASS[10],BILL[10],DUMMY[30];
char u1;
/*
#ifndef DEBUG
char C1_card_no[10]  = "12618444";
char C2_card_no[10]  = "12623314";
char C3_card_no[10]  = "12621567";
char M_card_no[10]   = "12533660";
char Atm_card_no[20] = "12534868";
#else
char C1_card_no[10]  = "96374568";
char C2_card_no[10]  = "17492084";
char C3_card_no[10]  = "05106529";
char M_card_no[10]   = "01267468";
char Atm_card_no[20] = "26768965";
#endif
*/
u32 bill = 0;
char *p;

int main()
{
	InitKPM();
	InitLCD();
	InitUART0();
	InitUART1();
	InitINT();

start:
		CmdLCD(CLEAR_LCD);
	disp_proj_name();
lstart:
	CmdLCD(CLEAR_LCD);
	PrintLCD(1,"CLICK ANY BUTTON");
	PrintLCD(2,"ENTRY DELETE EXIT");
	while(1)
	{
		while((!entryflag) && (!delflag) && (!exitflag));
		if(entryflag == 1)
		{	
			while(entryflag)
			{	
				CmdLCD(CLEAR_LCD);
				PrintLCD(1,"SCAN ITEM CARD");
				PrintLCD(2,"TO ADD ITEMS");
				entry_recv_wait();
				Disable_U1INT();
				if(entryflag)
				{	
					U0_TxStr(ID);
					CmdLCD(CLEAR_LCD);
					PrintLCD(1,"CARD : SCANNED");
					entry_pc_wait();
					switch(ID[0])
					{
						case '0': PrintLCD(2,ID+1); StrLCD(" NOT"); PrintLCD(3,"AVALIABLE IN STOCK"); break;
						case '1':	PrintLCD(2,ID+1); StrLCD(" ADDED"); PrintLCD(3,"SUCCESSFULLY"); 
											entry_pc_wait(); 
											PrintLCD(4,ID);
											delay_s(3);
											getbillfrompc();
											PrintLCD(1,"ToTal BILL : "); StrLCD(BILL);
											break;
						case '2': PrintLCD(2,ID+1); StrLCD(" : UPDATING"); PrintLCD(3,"QUANTITY"); 
											entry_pc_wait(); 
											PrintLCD(4,ID);
											delay_s(3);
											getbillfrompc();
											PrintLCD(1,"ToTal BILL : "); StrLCD(BILL);
											break;
						case 'M': CmdLCD(CLEAR_LCD); PrintLCD(2,"HELLO... MANAGER"); PrintLCD(3,"ACCESS GRANTED");
											entry_pc_wait();
											if(ID[0] == 'X')
											{
													CmdLCD(CLEAR_LCD); PrintLCD(2,"EXITING FROM ..."); PrintLCD(3,"MANAGER MODE");
													PrintLCD(4,"THANK YOU");
											} break;
					}
					delay_s(1);
					CmdLCD(CLEAR_LCD);		
				}	
			}
			entryflag=0;
			Enable_U1INT();
		}
		else if(delflag == 1)
		{
			getbillfrompc();
			if(!strcmp(BILL,"0.00"))
			{
				PrintLCD(1,"ERROR: DELETE");
				PrintLCD(2,"CART EMPTY");
				PrintLCD(3,"PLEASE ADD ITEMS");
				delflag = 0;
				delay_s(2);
				goto lstart;
			}
			while(delflag)
			{	
				CmdLCD(CLEAR_LCD);
				PrintLCD(1,"SCN CARD TO DEL ITEM");
				del_recv_wait();
				CmdLCD(CLEAR_LCD);
				if(delflag)
				{	
					PrintLCD(1,"CARD : SCANNED");
					U0_TxStr(ID);
					del_pc_wait();
					CmdLCD(CLEAR_LCD);
					switch(ID[0])
					{
						case '0': PrintLCD(2,"ERROR!!!   ITEM IS"); PrintLCD(3,"NOT IN YOUR CART"); PrintLCD(4,"PLEASE ADD ITEMS"); break;
						case '1':	PrintLCD(2,ID+1); StrLCD(" QUANTITY"); PrintLCD(3,"REDUCED SUCCESSFULLY"); 
											del_pc_wait(); 
											PrintLCD(4,ID);
											delay_s(3);
											getbillfrompc();
											PrintLCD(1,"ToTal BILL : "); StrLCD(BILL);
											break;
						case '2':	PrintLCD(2,ID+1); PrintLCD(3,"DELETED SUCCESSFULLY");
											del_pc_wait(); 
											PrintLCD(4,ID);
											delay_s(3);
											getbillfrompc();
											PrintLCD(1,"ToTal BILL : "); StrLCD(BILL);
											break;
						case 'X': PrintLCD(2,"ERROR!!!"); PrintLCD(3,"YOUR CART IS EMPTY"); PrintLCD(4,"PLEASE ADD ITEMS"); break;
					}
					delay_s(1);
					CmdLCD(CLEAR_LCD);
				}
			}
			delflag = 0;
		}
		else if(exitflag == 1)
		{
cashretry:
			CmdLCD(CLEAR_LCD);
			getbillfrompc();
			if(!strcmp(BILL,"0.00"))
			{
				PrintLCD(1,"CHECKOUT ERROR");
				PrintLCD(2,"CART EMPTY");
				PrintLCD(3,"PLEASE ADD ITEMS");
				exitflag = 0;
				delay_s(2);
				goto lstart;
			}
			PrintLCD(1,"ToTal BILL : "); StrLCD(BILL); //string of bill from PC
			PrintLCD(2,"** CHECK-OUT PAGE **");
			PrintLCD(3," ENTER:  1. CARD");
			PrintLCD(4," 2. CASH B. BACK");
			key = KeyScan();
			while(ColScan()==0);
			switch(key)
			{
				case '1': 
									switch(card_mode())
									{
										case 1: exitflag=0;
														delay_s(1);
														goto start;
										case 0: delay_s(2);
														goto cashretry;
										case -1: exitflag=0;
														goto start;
									}
									break;
				case '2': 
									switch(cash_mode())
									{
										case 1: exitflag=0;
														delay_s(1);
														goto start;
										case 0: delay_s(3);
														goto cashretry;
										case -1: exitflag=0;
														goto start;
									}
									break;
				case 'B': exitflag=0;
									goto lstart;
				default : CmdLCD(CLEAR_LCD); PrintLCD(2,"INVALID OPTION"); delay_s(1);
									goto cashretry;
			}
		}
	}
}

