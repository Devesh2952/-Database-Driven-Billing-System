#include <LPC21xx.h>
#include "types.h"
#include "delay.h"
#include "uart_defines.h"

void InitUART1(void)
{
	//cfg pin connect block
	//for p0.0/TXD0 & p0.1/RXD0
	PINSEL0|=0x00050000;
	//cfg U0LCR
	U1LCR=1<<DLAB_BIT|WORD_LEN_SEL_BITS;
	//cfg Baudrate generator
	U1DLM=BAUDLOADVAL>>8;
	U1DLL=BAUDLOADVAL;
	//clear DLAB bit
	U1LCR&=~(1<<DLAB_BIT);
}

void U1_TxChar(u8 sByte)
{
	//place data/byte to serialized out
	//into tx-buff;
	U1THR=sByte;
	//wait until transmission completion status
	while(((U1LSR>>TEMT_BIT)&1)==0);
}

u8 U1_RxChar(void)
{
	//wait until reception status
	while(((U1LSR>>DR_BIT)&1)==0);
	//read & recv data
	return U1RBR;
}

void U1_TxStr(s8 *s)
{
	while(*s)
	{
		U1_TxChar(*s++);
		delay_ms(5);
	}
}

void U1_TxU32(u32 n)
{
	u8 a[10];	s32 i=0;
	if(n==0)
		U1_TxChar('0');
	else
	{
		while(n)
		{
			a[i++]=(n%10)+48;
			n/=10;
    }	
    for(--i;i>=0;i--)
      U1_TxChar(a[i]);		
	}
}	

void U1_TxS32(s32 n)
{
	if(n<0)
	{
		U1_TxChar('-');
		n=-n;
	}
	U1_TxU32(n);
}

void U1_TxF32(f32 f,u32 nDP)
{
	u32 i,dp;
	if(f<0.0)
	{
		U1_TxChar('-');
		f=-f;		
	}		
	i=f;
	U1_TxU32(i);
	U1_TxChar('.');
	for(dp=0;dp<nDP;dp++)
	{
		f=(f-i)*10;
		i=f;
		U1_TxChar(i+48);
	}
}	

s8 * U1_RxStr(void)
{
	static s8 buff[11];
	s32 i=0;
	
	while(1)
	{
		buff[i]=U1_RxChar();
		U1_TxChar(buff[i]);
		if((buff[i]=='\n') || (buff[i]=='\r'))
		{
			buff[i]='\0';
			break;
    }
    i++;		
	}
	return buff;	
}
