#define FOSC 12000000 //Hz
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define BAUD 9600
#define BAUDLOADVAL (PCLK/(16 * BAUD))

//defines for UxLSR
#define DR_BIT   0
#define TEMT_BIT 6
//defines for UxLCR
#define WORD_LEN_SEL_BITS 3
#define DLAB_BIT          7
