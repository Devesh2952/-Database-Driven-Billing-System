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

extern int i,t,pass,chances,Amt,backflag;
extern char key,uflag0,uflag1,entryflag,exitflag,delflag,dummy,ch,flag;
extern char *p,ID[30],PASS[10],BILL[10],DUMMY[30];
extern char u1;

extern u32 bill;

void getbillfrompc()
{
			U0_TxStr("A$");
			atm_pc_wait();
			strcpy(BILL,ID);
}

int cash_mode()
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"-----CASH--MODE-----");
		delay_s(1);
		PrintLCD(1,"ENTER PAYING AMOUNT");
		Amt = kpm_input();
		if(atoi(ID) == Amt)
		{
			CmdLCD(CLEAR_LCD);
			PrintLCD(1,"TRANSACTION..");
			PrintLCD(2,"SUCCESS..");
			PrintLCD(3,"-----THANK YOU-----");
			PrintLCD(4,"----FOR VISTING----");
			delay_s(2);
			U0_TxStr("Q1$");
			exitflag = 0;
			return 1;
		}
		else if(Amt == -1)
		{
				CmdLCD(CLEAR_LCD);
				PrintLCD(1,"CASH TRANSACTION");
				PrintLCD(2,"CANCELLED");
				PrintLCD(3,"GOING BACK");
				delay_s(2);
				U0_TxStr("QX$");
				return 0;
		}
		else
		{
			CmdLCD(CLEAR_LCD);
			PrintLCD(1,"TRANSACTION..");
			PrintLCD(2,"FAILED..");
			PrintLCD(3,"INVALID AMOUNT");
			PrintLCD(4,"ENTER AGAIN");
			delay_s(2);
			U0_TxStr("Q0$");
			return 0;
		}
}

int card_mode()
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"CARD-MODE");
		PrintLCD(2,"SCAN ATM CARD");
		atm_recv_wait();
		CmdLCD(CLEAR_LCD);
		StrLCD("CARD SCANNED");
		PrintLCD(2,"Please WAIT...");
		delay_ms(500);
		U0_TxStr(ID); // SEND ATM ID TO PC 
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"TOTAL BILL : "); StrLCD(BILL);
		PrintLCD(2,"Waiting....");
		PrintLCD(3,"Confirm from PC");
		atm_pc_wait();
		CmdLCD(CLEAR_LCD);
		switch(ID[0])
		{
		case 'C': 
							if(passverify())
							{
									chances = 0;
									exitflag = 0;
									return 1;
							}
							else
							{
									if(backflag == 1) 
									{
										backflag =0;
										return 0;
									}
									if(chances >3)
									{
											CmdLCD(CLEAR_LCD);
											PrintLCD(1,"ATM LOCKED");
											PrintLCD(2,"EXITING...");
											delay_s(2);
											exitflag =0;
											chances =0;
											return -1;
									}
									CmdLCD(CLEAR_LCD);
									PrintLCD(2,"CHANCES : ");
									S32LCD(3-chances);
									PrintLCD(3,"REMAINING");
									delay_s(1);
									chances++;
									return 0;
							}
		  case 'B': blocked();
								return 0;
			case 'F': wrongatm();
								return 0;		
			case 'E': cancelled();
								break;								
		}
		return -1;
}

int passverify(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"ENTER PASSWORD");
		pass = kpm_input();
		if(pass == -1)
		{
				CmdLCD(CLEAR_LCD);
				PrintLCD(1,"ATM TRANSACTION");
				PrintLCD(2,"CANCELLED");
			PrintLCD(3,"GOING BACK");
				delay_s(1);
				backflag = 1;
				sprintf(PASS,"@%c$%c",'X','\0');
				//delay_s(1);
				U0_TxStr(PASS);
				return 0;
		}
		sprintf(PASS,"@%d$%c",pass,'\0');
		delay_s(1);
		U0_TxStr(PASS);
		atm_pc_wait();
		switch(ID[0])
		{	
			case 'S': success();
								return 1;			
			case 'F': failed();
								break;
		}
		return 0;		
}
void wrongatm(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"INVALID ATM CARD");
		PrintLCD(2,"SCAN ATM AGAIN");
		PrintLCD(3,"Please Wait ...");
		delay_s(2);
}
void blocked(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"CARD BLOCKED");
		PrintLCD(2,"TRY AGAIN LATER");
		delay_s(2);
}
void failed(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"TRANSACTION");
		PrintLCD(2,"FAILED...");
		delay_s(2);
}
void cancelled(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"TRANSACTION");
		PrintLCD(2,"CANCELLED...");
		delay_s(2);
}
void success(void)
{
		CmdLCD(CLEAR_LCD);
		PrintLCD(1,"TRANSACTION");
		PrintLCD(2,"SUCCESS...");
		delay_s(2);
}


void disp_proj_name()
{
	StrLCD("DATABASE-DRIVEN");
	CmdLCD(0xC0);
	StrLCD("BILLING SYSTEM");
	delay_s(1);
	CmdLCD(0x01);
}

void ID_Check_n_Modify(int from)
{	
#ifndef PRO
	if(from)
	{		
	/*		if((!(strcmp(ID,M_card_no))) && entryflag && !delflag && !exitflag)
			{
				//mystrmove(ID+1,ID,strlen(ID)+1);
				ID[0] = 'M';
				strcpy(ID+1,M_card_no);
				ID[9] = '$';
				ID[10] = '\0';
			}
			else 
			{*/
				if(entryflag)	DUMMY[0] = 'C';
				else if(delflag) DUMMY[0] = 'D';
				else if(exitflag) DUMMY[0] = 'B';
				strcpy(DUMMY+1,ID);
				strcpy(ID,DUMMY);
				ID[9] = '$';
				ID[10] = '\0';
	//		}
	}
	
#else
	if(((!(strcmp(ID,C1_card_no))) || (!(strcmp(ID,C2_card_no))) || (!(strcmp(ID,C3_card_no)))) && (entryflag || delflag) && (exitflag ==0))
	{
		mystrmove(ID+1,ID,strlen(ID)+1);
		if(entryflag) ID[0] = 'C';
		if(delflag) ID[0] = 'D';
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,M_card_no))) && entryflag && !delflag && !exitflag)
	{
		mystrmove(ID+1,ID,strlen(ID)+1);
		ID[0] = 'M';
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,Atm_card_no))) && exitflag && !entryflag && !delflag)
	{
		mystrmove(ID+1,ID,strlen(ID)+1);
		ID[0] = 'B';
		ID[9] = '$';
//		ID[9] = 'A';
		ID[10] = '\0';	
//		sprintf(ID,"%s%d%s",ID,bill,"$");
	}
#endif
}

void entry_recv_wait()
{
		ClearBufferUART(1);
	  while(uflag1 == 0 && entryflag);
		uflag1 = 0;
		if(entryflag == 1) ID_Check_n_Modify(1);
}

void del_recv_wait()
{
		ClearBufferUART(1);
		while(uflag1 == 0 && delflag);
		uflag1 = 0;
		if(delflag == 1) ID_Check_n_Modify(1);
}

void atm_recv_wait()
{
		ClearBufferUART(1);
		while(uflag1 == 0 && exitflag);
		uflag1 = 0;
		if(exitflag == 1) ID_Check_n_Modify(1);
}

void entry_pc_wait()
{
		ClearBufferUART(0);
		//PrintLCD(2,"1");
		while(uflag0 == 0 && entryflag);
		//PrintLCD(3,"2");
		uflag0 = 0;
	//	PrintLCD(4,"3");
		if(entryflag == 1) ID_Check_n_Modify(0);
}

void del_pc_wait()
{
		ClearBufferUART(0);
		while(uflag0 == 0 && delflag);
		uflag0 = 0;
		if(delflag == 1) ID_Check_n_Modify(0);
}

void atm_pc_wait()
{
		ClearBufferUART(0);
		while(uflag0 == 0 && (exitflag || delflag || entryflag));
		uflag0 = 0;
		if(exitflag == 1 || delflag == 1 || entryflag == 1) ID_Check_n_Modify(0);
}

void mystrmove(char *dest,char *src,int len)
{
	int i;
	char temp[20];
	strcpy(temp,ID);
	for(i=0;i<len;i++)
	{
		dest[i] = temp[i];
	}
	return;
}
