void InitINT(void);
void Init_UARTINT(void);
void UART0_isr(void) __irq;
void entry_isr(void) __irq;
void del_isr(void) __irq;
void exit_isr(void) __irq;
void ClearBufferUART(int);
void Enable_U1INT(void);
void Disable_U1INT(void);
