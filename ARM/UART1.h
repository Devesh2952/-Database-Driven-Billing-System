//uart1.h
void InitUART1(void);
void U1_TxChar(u8 sByte);
u8 U1_RxChar(void);
void U1_TxStr(s8 *);
void U1_TxU32(u32);
void U1_TxS32(s32);
void U1_TxF32(f32,u32);
s8 * U1_RxStr(void);
