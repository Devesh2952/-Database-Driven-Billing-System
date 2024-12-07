//delay.c
#include "types.h"
void delay_us(u32 dlyUS)
{
	 //@FOSC=12MHz,CCLK=60MHz
	 //for(dlyUS*=12;dlyUS>0;dlyUS--);
   dlyUS*=12;
	 while(dlyUS--);	
}

void delay_ms(u32 dlyMS)
{
	 //@FOSC=12MHz,CCLK=60MHz
	 //for(dlyMS*=12000;dlyMS>0;dlyMS--);
   dlyMS*=12000;
	 while(dlyMS--);	
}

void delay_s(u32 dlyS)
{
	 //@FOSC=12MHz,CCLK=60MHz
	 //for(dlyS*=12000000;dlyUS>0;dlyUS--);
   dlyS*=12000000;
	 while(dlyS--);	
}
