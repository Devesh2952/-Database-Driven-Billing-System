//kpm2.c
#include <LPC21xx.h>
#include "types.h"
#include "kpm_defines.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "delay.h"
extern char key;
u8 kpmLUT[4][4]=
{
	{ '1','2','3','A' },
	{ '4','5','6','B' },
	{ '7','8','9','C' },
	{ '*','0','#','D' }
};	

u32 ColScan(void)
{
	return (((IOPIN1>>COL0)&15)==15)?1:0;
}	

void InitKPM(void)
{
	//cfg row lines as gpio out
	IODIR1|=15<<ROW0;
	//& col lines are default input
}	

u32 RowCheck(void)
{
	u32 rNo;
	for(rNo=0;rNo<4;rNo++)
	{
	 //ground 0th to 3rd row iteratively
	 IOPIN1=((IOPIN1&~(15<<ROW0))|(((~(1<<rNo))&15)<<ROW0));	
	  //chk if key was pressed in this row
	 if(ColScan()==0)
	 {
		 break;
	 }
  }	 
	//re-initializing all rows to ground
	IOCLR1=15<<ROW0;
	return rNo;
}	

u32 ColCheck(void)
{
	u32 cNo;
	for(cNo=0;cNo<4;cNo++)
	{
	  if(((IOPIN1>>(COL0+cNo))&1)==0)
     	break;
	}	
	return cNo;
}

u32 KeyScan(void)
{
	u32 rNo,cNo;
	//wait for any key press
	while(ColScan());
	//identify row in which key was pressed
	rNo=RowCheck();
	//identify col in which key was pressed
	cNo=ColCheck();
	//extract/map key pressed to key value
	return kpmLUT[rNo][cNo];
}	

//////////KPM INPUT FUNC//////////
int kpm_input()
{
	int pass= 0;
	CmdLCD(GOTO_LINE2_POS0);
	while(1)
	{
			key=KeyScan();
			delay_ms(100);
			if(pass && key == 'D')
			{
				CmdLCD(GOTO_LINE1_POS0);
				CmdLCD(CLEAR_LCD);
				break;
			}
			else if(key == 'B')
			{
				return -1;
			}
			else if(key == '*' || key == 'A' || key == '#')
			{
					PrintLCD(3,"Invalid symbol");
				PrintLCD(4,"USE 0-9,B,C,D");
					delay_s(1);
					ClearLineLCD(3);
					ClearLineLCD(4);
			}
			else
			{	
				if(key>='0' && key <= '9') 
				{	
					pass = (pass *10)+(key-48);
					CmdLCD(GOTO_LINE2_POS0);
					if(pass>99999999)
					{
						CmdLCD(GOTO_LINE1_POS0);
						StrLCD("ONLY 8 DIGITS  ");
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
	PrintLCD(2,"PLEASE WAIT...");
	PrintLCD(3,"PROCESSING... ");
	return pass;
}
///////////// KPM FUNC END ////////////////
