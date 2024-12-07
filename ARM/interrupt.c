//#define DEBUG

#include <LPC21xx.h>
#include "types.h"
#include "interrupt_defines.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "delay.h"
#include "interrupt.h"
#include "string.h"


extern char ID[30],uflag0,uflag1,dummy,ch,entryflag,exitflag,delflag;
extern int i;
void UART0_isr(void) __irq
{
	if((U0IIR & 0x04)) //check if receive interrupt
  	{
		ch = U0RBR;	/* Read to Clear Receive Interrupt */
		if(ch != '$')
		{
			if(ch == '@') i=0;
			else ID[i++] = ch;
		}
		else
		{
			ID[i] = '\0';
			i=0;
			uflag0 = 1;
		}
  }
  else
  {
      dummy=U0IIR; //Read to Clear transmit interrupt
  }
	VICVectAddr = 0; /* dummy write */
}

void ClearBufferUART(int u)
{
	int j=0;
	for(j=0;j<30;j++)	ID[j] = 0;
	if(u) uflag1 = 0;
	else uflag0 = 0;
}

void UART1_isr(void) __irq
{
	
	if((U1IIR & 0x04)) //check if receive interrupt
  	{
		ch = U1RBR;	/* Read to Clear Receive Interrupt */
#ifdef DEBUG
		if(ch != '$')
		{
			if(ch == '@') i=0;
#else 
		if(ch != 0x03)
		{
			if(ch == 0x02) i=0;
#endif
			else ID[i++] = ch;
		}
		else
		{
			ID[i] = '\0';
			i=0;
			uflag1 = 1;
		}
  }
  else
  {
      dummy=U1IIR; //Read to Clear transmit interrupt
  }
	VICVectAddr = 0; /* dummy write */	
}

void entry_isr(void) __irq		//EINT0
{
	entryflag = 1;
	delflag = 0;
	exitflag =0;
	EXTINT |= 1<<0; 
	VICVectAddr = 0;
}

void del_isr(void) __irq		 //EINT1
{
		entryflag = 0;
		delflag = 1;
		exitflag = 0;
	EXTINT |= 1<<1;
	VICVectAddr = 0;
}

void exit_isr(void) __irq		   //EINT2
{
		entryflag = 0;
		delflag = 0;
		exitflag =1;
	EXTINT |= 1<<2;
	VICVectAddr = 0;
}

void InitINT(void)
{
	PINSEL0 = (PINSEL0 &~(0xA0000000))|(0xA0000000); //config EINT1, EINT2
	PINSEL1 = (PINSEL1 &~(0x00000001))|(0x00000001); //config EINT0
	VICIntSelect = 0;
	VICIntEnable = (1<<EINT0)|(1<<EINT1)|(1<<EINT2)|(1<<UART0INT)|(1<<UART1INT); //Enabling uart & external interrupts(0,1,2)
	
	VICVectCntl0 = (1<<5)| UART1INT;
	VICVectCntl1 = (1<<5)| UART0INT;
	VICVectCntl2 = (1<<5)| EINT0;
	VICVectCntl3 = (1<<5)| EINT1;
	VICVectCntl4 = (1<<5)| EINT2;
	
	VICVectAddr0 =  (u32)UART1_isr;
	VICVectAddr1 =  (u32)UART0_isr;
	VICVectAddr2 = 	(u32)entry_isr;
	VICVectAddr3 = 	(u32)del_isr;
	VICVectAddr4 = 	(u32)exit_isr;
	
	Init_UARTINT();
	//EINT triggering mode
	EXTMODE = (1<<0)|(1<<1)|(1<<2);
	EXTPOLAR = 0; //negative edge triggering
}
void Init_UARTINT(void)
{	
	U0IER = 0x01;  // 0x01 only RBR INTERRUPT enable, 0x03 BOTH RBR & THRE interrupts enable
	U1IER = 0x01;
}

void Enable_U1INT()
{
VICIntEnable|=(1<<UART1INT);
}

void Disable_U1INT()
{
VICIntEnable&=~(1<<UART1INT);
}
