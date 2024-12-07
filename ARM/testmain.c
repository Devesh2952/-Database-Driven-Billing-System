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
#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#define PRO
#define DEBUG
void mystrmove(char*,char*,int);

int i,t,pass=0,chances =0;;
char key,uflag0 = 0,uflag1 = 0,entryflag=0,exitflag =0,delflag=0,dummy,ch,flag=0;
char ID[20]  = "HELLO",PASS[10];
char u1;
#ifndef DEBUG
char C1_card_no[10]  = "00318444";
char C2_card_no[10]  = "00315103";
char C3_card_no[10]  = "00320524";
char M_card_no[10]   = "12540752";
char Atm_card_no[20] = "12638593";
#else
char C1_card_no[10]  = "96374568";
char C2_card_no[10]  = "17492084";
char C3_card_no[10]  = "05106529";
char M_card_no[10]   = "01267468";
char Atm_card_no[20] = "26768965";
#endif
u32 bill = 945;
char *p;
//////////KPM INPUT FUNC////
int kpm_input()
{
	unsigned long long int pass= 0;
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("ENTER PASSWORD");
	CmdLCD(GOTO_LINE2_POS0);
	while(1)
	{
		
			key=KeyScan();
			delay_ms(200);
			if(pass && key == 'D')
			{
				CmdLCD(GOTO_LINE1_POS0);
				CmdLCD(CLEAR_LCD);
				StrLCD("WAIT for ...");
				chances++;
				break;
			}
			else if(key == 'B')
			{
				return -1;
			}
			else
			{	
				if(key>='0' && key <= '9') 
				{	
					pass = (pass *10)+(key-48);
					CmdLCD(GOTO_LINE2_POS0);
					if(pass>9999999999999999)
					{
						CmdLCD(GOTO_LINE1_POS0);
						StrLCD("ONLY 16 DIGITS  ");
						pass/=10; 
					}
					CmdLCD(GOTO_LINE2_POS0);
					U64LCD(pass);
				}
				else if(key=='C')
				{
					CmdLCD(GOTO_LINE2_POS0);
					StrLCD("                ");
					pass/=10;
					if(pass==0) 
					{
						CmdLCD(GOTO_LINE2_POS0);
						StrLCD("  ");
						CmdLCD(GOTO_LINE2_POS0);
					}
					else
					{
						CmdLCD(GOTO_LINE2_POS0);
						U64LCD(pass);
					}
				}
			}
			while(ColScan()==0);
	}
	return pass;
}
///////////// KPM FUNC END ////////////////

void disp_proj_name()
{
	StrLCD("DATABASE-DRIVEN");
	CmdLCD(0xC0);
	StrLCD("BILLING SYSTEM");
	delay_s(1);
	CmdLCD(0x01);
}

void RFID_read() // @12345678$
{
	StrLCD("In RFID READ : ");
	u1 = U1_RxChar();
#ifndef PRO	
	if(u1 == 0x02)
#else
	if(u1 == '@')
#endif
	{
		i=0;
#ifndef PRO		
		while((u1 = U1_RxChar())!= 0x03)
#else
		while((u1 = U1_RxChar())!= '$')
#endif		
		{
			 ID[i++] = u1;
		}
		ID[i] = '\0';
		i=0;	
	}
}

void ID_Check_n_Modify(void)
{	
#ifdef PRO

	if((!(strcmp(ID,C1_card_no))) && (entryflag || delflag) && (exitflag ==0))
	{
		if(entryflag) ID[0] = 'C';
		if(delflag) ID[0] = 'D';
		strcpy(ID+1,C1_card_no);
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,C2_card_no))) && (entryflag || delflag) && (exitflag ==0))
	{
		if(entryflag) ID[0] = 'C';
		if(delflag) ID[0] = 'D';
		strcpy(ID+1,C2_card_no);
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,C3_card_no))) && (entryflag || delflag) && (exitflag ==0))
	{
		if(entryflag) ID[0] = 'C';
		if(delflag) ID[0] = 'D';
		strcpy(ID+1,C3_card_no);
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,M_card_no))) && entryflag && !delflag && !exitflag)
	{
		//mystrmove(ID+1,ID,strlen(ID)+1);
		ID[0] = 'M';
		strcpy(ID+1,M_card_no);
		ID[9] = '$';
		ID[10] = '\0';
	}
	else if((!(strcmp(ID,Atm_card_no))) && exitflag)
	{
		//mystrmove(ID+1,ID,strlen(ID)+1);
		ID[0] = 'B';
		strcpy(ID+1,Atm_card_no);
		ID[9] = '$';
//		ID[9] = 'A';
		ID[10] = '\0';	
//		sprintf(ID,"%s%d%s",ID,bill,"$");
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
void entry_func()
{
	  while(uflag1 == 0 && entryflag);
		uflag1 = 0;
		ID_Check_n_Modify();
}

void del_func()
{
		while(uflag1 == 0 && delflag);
		uflag1 = 0;
		ID_Check_n_Modify();
}

void exit_func()
{
		while(uflag1 == 0 && exitflag);
		uflag1 = 0;
		ID_Check_n_Modify();
}

int main()
{	
	InitKPM();
	InitLCD();
	InitUART0();
	InitUART1();
	InitINT();
	
	disp_proj_name();
	StrLCD("Click interrupt");
	while(1)
	{
		while((!entryflag) && (!delflag) && (!exitflag));
		//U0_TxStr(ID);
		if(entryflag == 1)
		{
			while(entryflag)
			{	
				CmdLCD(CLEAR_LCD);
				entry_func();
				if(entryflag)
				{	
					U0_TxStr(ID);
					CmdLCD(CLEAR_LCD);
					StrLCD(ID);
					StrLCD(" SUCCESS");
					delay_s(1);
					CmdLCD(CLEAR_LCD);
				}	
			}
			entryflag=0;
		}
		else if(delflag == 1)
		{
			while(delflag)
			{	
				CmdLCD(CLEAR_LCD);
				StrLCD("SCN CARD TO DEL ITEM");
				CmdLCD(0xC0);
				del_func();
				if(delflag)
				{	
					U0_TxStr(ID);
					CmdLCD(CLEAR_LCD);
					StrLCD("ITEM DELETED SUCCESSFULLY");
					StrLCD(ID);
					StrLCD(" SUCCESS");
					delay_s(1);
					CmdLCD(CLEAR_LCD);
				}
			}
			delflag = 0;
		}
		else if(exitflag == 1)
		{
				if(flag == 0)
				{
					U0_TxStr("A");
					while(uflag0 == 0);
					uflag0 = 0;
					CmdLCD(0x01);
					CmdLCD(0x80);
					StrLCD("ToTal BILL : ");
					StrLCD(ID);
					CmdLCD(0xC0);
					StrLCD("** CHECK-OUT PAGE **");
					CmdLCD(0x94);
					StrLCD("    ENTER    ");
					CmdLCD(0xD4);
					StrLCD("1. CARD  2. CASH");
					flag =1;
				}
				else
				{
keyAgain:	key = KeyScan();
					while(ColScan()==0);
					switch(key)
					{
						case '1':  
							CmdLCD(CLEAR_LCD);
							StrLCD("CARD-MODE");
							CmdLCD(0xC0);
							StrLCD("SCAN ATM CARD");
							exit_func();
							CmdLCD(CLEAR_LCD);
							U0_TxStr(ID);
							StrLCD("CARD SCAN SUCCESS");
							delay_s(1);
							CmdLCD(CLEAR_LCD);
							StrLCD("TOTAL BILL : ");
							StrLCD(ID);
								CmdLCD(0xC0);
								StrLCD("Waiting....");
							CmdLCD(0x94);
								StrLCD("...to Confirm");
							while(uflag0==0);
							uflag0=0;
							if(ID[0] == 'C')
							{	
retry:					CmdLCD(0x01);
								pass = kpm_input();
								sprintf(PASS,"@%d$",pass);
								U1_TxStr(PASS);
								delay_s(1);
								U0_TxStr(PASS);
								while(uflag0==0);
								uflag0=0;
								if(ID[0] == 'S')
								{
									CmdLCD(0x01);
									StrLCD("TRANSACTION");
									CmdLCD(0xC0);
									StrLCD("SUCCESS...");
									delay_s(2);
									chances =0;
									exitflag =0;
									break;
								}
								else if(ID[0] == 'F')
								{
									CmdLCD(0x01);
									StrLCD("TRANSACTION");
									CmdLCD(0xC0);
									StrLCD("FAILED...");
									delay_s(2);
									if(chances >=3)
									{
										CmdLCD(0x01);
										StrLCD("ATM LOCKED");
										CmdLCD(0xC0);
										StrLCD("EXITING...");
										exitflag =0;
										delay_s(2);
										break;
									}
									else goto retry;
								}
							}
							else if(ID[0] == 'E')
							{
								CmdLCD(0x01);
								StrLCD("TRANSACTION");
								CmdLCD(0xC0);
								StrLCD("CANCELLED...");
							}	
							
							break;
						case '2':	 
							CmdLCD(CLEAR_LCD);
							StrLCD("CASH"); 
							delay_s(2);
							break;

						default: goto keyAgain;
					}
					exitflag = 0;
				}
			
			
	 }

	}
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
