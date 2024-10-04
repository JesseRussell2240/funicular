//UART Header

//Header Guard

#ifndef _STM32F303RE_UART_E
#define _STM32F303RE_UART_E

#define BAUD_RATE 9600 //configure this for faster com speed

void UART1_Init(void);
void UART1putc(char thisChar);
void UART1puts(char *thisString);
char UART1getc(void);
void UART1printf( char *fmt, ... );
char UART1getcNB(void);
void USART1_IRQHandler(void);

#endif
