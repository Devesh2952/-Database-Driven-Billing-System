#ifndef _BANK_H_
#define _BANK_H_

typedef struct bank
{
    char ID[10];
    char *username;
    char *bankname;
    char *AccNo;
    char *IFSC;
    float balance;
    char *place;
    char stat[20];
    int PIN;
    struct bank *next;
}BANK;
void SaveBANK(BANK *h);
void SyncBANK();
void Print_Bank_Details(BANK *h,char ch);
BANK* CheckATM(char* ID);
int CheckPass(char *pass);
int DeductAmt(float bill);
#endif
