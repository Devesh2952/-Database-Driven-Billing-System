#ifndef _MYFUNCS_H_
#define _MYFUNCS_H_

void mystrmove(char*,char*,int);
void disp_proj_name(void);
void ID_Check_n_Modify(int);
void entry_recv_wait(void);
void del_recv_wait(void);
void atm_recv_wait(void);
void atm_pc_wait(void);
void entry_pc_wait(void);
void del_pc_wait(void);
int cash_mode(void);
int card_mode(void);
void getbillfrompc(void);
int passverify(void);
void failed(void);
void cancelled(void);
void success(void);
void blocked(void);
void wrongatm(void);

#endif
