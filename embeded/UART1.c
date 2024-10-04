#include "UART1.h"  // Change to appropriate header file
#include "utility.h"
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_UART1_BUFSIZ 60

void UART1_Init(void){
    // Part 1: Clock source config
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock (APB2 for USART1)

    RCC->CFGR3 &= ~RCC_CFGR3_USART1SW_1;
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_0; // Select System clock for USART1

    // Part 2: GPIO config (PA9 - TX, PA10 - RX for USART1)
    ENABLE_CLOCKx(A); // Enable GPIOA clock
    MODER_SET(A, 9, MODER_AF);  // Set PA9 to AF (Alternate Function)
    MODER_SET(A, 10, MODER_AF); // Set PA10 to AF (Alternate Function)

    GPIO_AFR_SET(A, 9, 7);  // Set alternate function 7 (USART1) for PA9
    GPIO_AFR_SET(A, 10, 7); // Set alternate function 7 (USART1) for PA10

    // Configure GPIO output speed and pull-up/pull-down
    GPIOA->OSPEEDR &= ~(0x3UL << (9*2));  // Low speed for PA9
    GPIOA->OSPEEDR &= ~(0x3UL << (10*2)); // Low speed for PA10
    PUPDR_SET(A, 9, PUPDR_NP);  // No Pull-up/pull-down for PA9
    PUPDR_SET(A, 10, PUPDR_NP); // No Pull-up/pull-down for PA10

    // Part 3: UART1 config (9600 BAUD)
    USART1->CR1 &= ~USART_CR1_UE;  // Disable USART1
    USART1->CR1 &= ~USART_CR1_M;   // 8-bit data
    USART1->CR1 &= ~USART_CR1_OVER8; // 16x oversampling
    USART1->CR2 &= ~USART_CR2_STOP;  // 1 stop bit
    USART1->BRR = SystemCoreClock / 9600; // Set Baud rate to 9600

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable TX and RX

    // Enable the USART1 receive interrupt
    USART1->CR1 |= USART_CR1_RXNEIE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;

    // Wait until TEACK and REACK are set
    while ((USART1->ISR & USART_ISR_TEACK) == 0);
    while ((USART1->ISR & USART_ISR_REACK) == 0);

    // Enable USART1 interrupt in NVIC
    NVIC_EnableIRQ(USART1_IRQn);
}

// Interrupt handler for USART1
void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE) {
        // Read the received data
        char receivedChar = USART1->RDR;
        // Handle the received character
        UART1printf("Received: %c\n", receivedChar);
    }
}

void UART1putc(char thisChar){
    // Wait until the Transmit Data Register (TDR) is empty
    while ((USART1->ISR & USART_ISR_TXE) == 0);
    // Send the character by writing it to the TDR
    USART1->TDR = (uint8_t)thisChar;
}

void UART1puts(char *thisString) {
    while (*thisString) {
        UART1putc(*thisString++);
    }
    UART1putc('\n');
}

char UART1getc(void) {
    while ((USART1->ISR & USART_ISR_RXNE) == 0);
    return (char)(USART1->RDR);
}

void UART1printf(char *fmt, ...) {
    va_list args;
    char buffer[MAX_UART1_BUFSIZ];

    va_start(args, fmt);
    vsnprintf(buffer, MAX_UART1_BUFSIZ, fmt, args);
    va_end(args);
    
    UART1puts(buffer);
}

char UART1getcNB(void) {
    if (USART1->ISR & USART_ISR_RXNE)
        return (USART1->RDR);
    else
        return '\0';
}
