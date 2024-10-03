//UART Header

//Header Guard

#ifndef _STM32F303RE_UART_E
#define _STM32F303RE_UART_E

#define BAUD_RATE 9600 //configure this for faster com speed

void UART2_Init(void);
void UARTputc(char thisChar);
void UARTputs(char *thisString);
char UARTgetc(void);
void UARTprintf( char *fmt, ... );
char UARTgetcNB(void);

#endif
