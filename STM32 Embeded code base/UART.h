////////////////////
//UART Module Header
////////////////////

#ifndef _STM32F303RE_UART_E
#define _STM32F303RE_UART_E
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>

// Define buffer size only once
#define MAX_UART_BUFSIZ 64

// Baud rate definition
#define BAUD_RATE 9600

// Function declarations
int UARTPI_GetChar (void);
void UARTPI_SendChar (char c);
void UARTPI_Init(void);
void UART2_Init(void);
void UARTputc(char thisChar);
void UARTputs(char *str);
char UARTgetc(void);
void UARTprintf(char *fmt, ...);
char UARTgetcNB(void);
void UARTgets(char *buffer, size_t maxLength);
void USART2_IRQHandler(void);

// Global Buffers and Indices
volatile char txBuffer[MAX_UART_BUFSIZ];
volatile int txWriteIndex = 0, txReadIndex = 0;
volatile char rxBuffer[MAX_UART_BUFSIZ];
volatile int rxWriteIndex = 0, rxReadIndex = 0;

#endif
