#ifndef _UART_H_
#define _UART_H_

int Init_Serial(void);
int   serialOpen      (const char *device, const int baud) ;
void  serialClose     (const int fd) ;
void  serialFlush     (const int fd) ;
void  serialPutchar   (const int fd, const unsigned char c) ;
int   serialGetchar   (const int fd) ;
char* serialGetStr    (const int fd) ;
void serialPutStr (const int fd, char *str);
#endif
