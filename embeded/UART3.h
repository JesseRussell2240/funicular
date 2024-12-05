//UART Header

//Header Guard

#ifndef _STM32F303RE_UART_E
#define _STM32F303RE_UART_E

#define BAUD_RATE 9600 //configure this for faster com speed

void UART3_Init(void);
void UART3putc(char thisChar);
void UART3puts(char *thisString);
char UART3getc(void);
void UART3printf( char *fmt, ... );
char UART3getcNB(void);
void USART3_IRQHandler(void);
void ProcessReceivedString(char *input);

#endif
