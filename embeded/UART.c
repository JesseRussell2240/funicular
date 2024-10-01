// UART setup for STM32. Performs bit shifting and register initializations for UART2.
// Jesse Russell, Lab 1 for Embedded Software Systems

#include "UART.h"
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_UART_BUFSIZ 64

volatile char txBuffer[MAX_UART_BUFSIZ];
volatile char rxBuffer[MAX_UART_BUFSIZ];
volatile int txWriteIndex = 0;
volatile int txReadIndex = 0;
volatile int rxWriteIndex = 0;
volatile int rxReadIndex = 0;


void UART2_Init(void)
{
    // Enable USART2 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure SYSCLK
    RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_1;
    RCC->CFGR3 |= RCC_CFGR3_USART2SW_0;

    // Enable Port A clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Configure PA2 and PA3 as alternative functions
    GPIOA->MODER &= ~(0x03UL << (2*2));
    GPIOA->MODER &= ~(0x03UL << (2*3));
    GPIOA->MODER |= (0x02UL << (2*2));
    GPIOA->MODER |= (0x02UL << (2*3));
    GPIOA->AFR[0] |= (0x07UL << (4*2));
    GPIOA->AFR[0] |= (0x07UL << (4*3));

    // Set low speed, no pull-up, push-pull configuration for PA2 and PA3
    GPIOA->OSPEEDR &= ~(0x3UL << (2*2));
    GPIOA->OSPEEDR &= ~(0x3UL << (2*3));
    GPIOA->PUPDR &= ~(0x3UL << (2*2));
    GPIOA->PUPDR &= ~(0x3UL << (2*3));
    GPIOA->OTYPER &= ~(0x1UL << 2);
    GPIOA->OTYPER &= ~(0x1UL << 3);

    // Disable USART2
    USART2->CR1 &= ~USART_CR1_UE;

    // Set USART2 for 8 data bits, 1 stop bit, oversampling by 16
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR1 &= ~USART_CR1_OVER8;
    USART2->CR2 &= ~USART_CR2_STOP;

    // Set baud rate
    USART2->BRR = SystemCoreClock / BAUD_RATE;

    // Enable transmitter and receiver
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

    // Enable interrupts for receive and transmit
    USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_TXEIE;

    // Re-enable USART
    USART2->CR1 |= USART_CR1_UE;

    // Enable NVIC interrupt for USART2
    NVIC_EnableIRQ(USART2_IRQn);

    // Wait for USART to be ready
    while (!(USART2->ISR & USART_ISR_TEACK));
    while (!(USART2->ISR & USART_ISR_REACK));
}

void USART2_IRQHandler(void)
{
    // Handle receive interrupt
    if (USART2->ISR & USART_ISR_RXNE) {
        char receivedChar = (char)(USART2->RDR);
        int nextIndex = (rxWriteIndex + 1) % MAX_UART_BUFSIZ;
        if (nextIndex != rxReadIndex) {
            rxBuffer[rxWriteIndex] = receivedChar;
            rxWriteIndex = nextIndex;
        }
    }

    // Handle transmit interrupt
    if (USART2->ISR & USART_ISR_TXE) {
        if (txReadIndex != txWriteIndex) {
            USART2->TDR = txBuffer[txReadIndex];
            txReadIndex = (txReadIndex + 1) % MAX_UART_BUFSIZ;
        } else {
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

void UARTputc(char thisChar)
{
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = (uint8_t)thisChar;
    USART2->CR1 |= USART_CR1_TXEIE;  // Re-enable TXE interrupt to continue transmitting
}

void UARTputs(char *str)
{
    while (*str) {
        UARTputc(*str++);
    }
}

char UARTgetc(void)
{
    while (!(USART2->ISR & USART_ISR_RXNE));
    return (char)(USART2->RDR);
}

char UARTgetcNB(void)
{
    if (USART2->ISR & USART_ISR_RXNE)
        return (char)(USART2->RDR);
    else
        return '\0';
}

void UARTgets(char *buffer, size_t maxLength)
{
    size_t index = 0;
    while (1) {
        char receivedChar = UARTgetc();

        if (receivedChar == '\n' || receivedChar == '\r') {
            buffer[index] = '\0'; // Null-terminate the string
            break;
        }

        if (index < maxLength - 1) {
            buffer[index++] = receivedChar;
        }
    }
}

void UARTprintf(char *fmt, ...)
{
    va_list args;
    char buffer[MAX_UART_BUFSIZ];

    va_start(args, fmt);
    vsnprintf(buffer, MAX_UART_BUFSIZ, fmt, args);
    va_end(args);

    UARTputs(buffer);
}
