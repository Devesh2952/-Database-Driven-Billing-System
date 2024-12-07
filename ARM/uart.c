#include <LPC21xx.h>
#include "types.h"
#include "delay.h"
#include "uart_defines.h"

void InitUART0(void)
{
	//cfg pin connect block
	//for p0.0/TXD0 & p0.1/RXD0
	PINSEL0|=0x00000005;
	//cfg U0LCR
	U0LCR=1<<DLAB_BIT|WORD_LEN_SEL_BITS;
	//cfg Baudrate generator
	U0DLM=BAUDLOADVAL>>8;
	U0DLL=BAUDLOADVAL;
	//clear DLAB bit
	U0LCR&=~(1<<DLAB_BIT);
}

void U0_TxChar(u8 sByte)
{
	//place data/byte to serialized out
	//into tx-buff;
	U0THR=sByte;
	//wait until transmission completion status
	while(((U0LSR>>TEMT_BIT)&1)==0);
}

u8 U0_RxChar(void)
{
	//wait until reception status
	while(((U0LSR>>DR_BIT)&1)==0);
	//read & recv data
	return U0RBR;
}

void U0_TxStr(s8 *s)
{
	while(*s)
	{	
		U0_TxChar(*s++);
		delay_ms(50);
	}
}

void U0_TxU32(u32 n)
{
	u8 a[10];	s32 i=0;
	if(n==0)
		U0_TxChar('0');
	else
	{
		while(n)
		{
			a[i++]=(n%10)+48;
			n/=10;
    }	
    for(--i;i>=0;i--)
      U0_TxChar(a[i]);		
	}
}	

void U0_TxS32(s32 n)
{
	if(n<0)
	{
		U0_TxChar('-');
		n=-n;
	}
	U0_TxU32(n);
}

void U0_TxF32(f32 f,u32 nDP)
{
	u32 i,dp;
	if(f<0.0)
	{
		U0_TxChar('-');
		f=-f;		
	}		
	i=f;
	U0_TxU32(i);
	U0_TxChar('.');
	for(dp=0;dp<nDP;dp++)
	{
		f=(f-i)*10;
		i=f;
		U0_TxChar(i+48);
	}
}	

s8 * U0_RxStr(void)
{
	static s8 buff[100];
	s32 i=0;
	while(1)
	{
		buff[i]=U0_RxChar();
		U0_TxChar(buff[i]);
		if((buff[i]=='\n') || (buff[i]=='\r'))
		{
			buff[i]='\0';
			break;
    }
    i++;		
	}
	return buff;	
}
