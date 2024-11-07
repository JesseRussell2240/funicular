/*#include "UART3.h"
#include "utility.h"
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_UART3_BUFSIZ 60

char rxBuffer[MAX_UART3_BUFSIZ];  // Buffer to store the incoming string
static volatile uint8_t rxIndex = 0;     // Index to track the buffer position


void UART3_Init(void){
    // Part 1: clock source config
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable USART3 clock

    RCC->CFGR3 &= ~RCC_CFGR3_USART3SW_1;
    RCC->CFGR3 |= RCC_CFGR3_USART3SW_0; // Select System clock for USART3

    // Part 2: GPIO config (PB10 - TX, PB11 - RX for USART3)-100
    ENABLE_CLOCKx(B);
    MODER_SET(B, 10, MODER_AF); // Set PB10 to AF
    MODER_SET(B, 11, MODER_AF); // Set PB11 to AF

    GPIO_AFR_SET(B, 10, 7); // Set alternate function for USART3
    GPIO_AFR_SET(B, 11, 7);

    // Configure GPIO output speed and pull-up/pull-down
    GPIOB->OSPEEDR &= ~(0x3UL << (10*2)); // Low speed
    GPIOB->OSPEEDR &= ~(0x3UL << (11*2)); // Low speed
    PUPDR_SET(B, 10, PUPDR_NP); // No Pull
    PUPDR_SET(B, 11, PUPDR_NP);

    // Part 3: UART3 config (9600 BAUD)
    USART3->CR1 &= ~USART_CR1_UE;  // Disable USART
    USART3->CR1 &= ~USART_CR1_M;   // 8-bit data
    USART3->CR1 &= ~USART_CR1_OVER8; // 16x oversampling
    USART3->CR2 &= ~USART_CR2_STOP;  // 1 stop bit
    USART3->BRR = SystemCoreClock / 9600; // Set Baud rate to 9600

    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable TX and RX

    // Enable the USART3 receive interrupt
    USART3->CR1 |= USART_CR1_RXNEIE;

    // Enable USART3
    USART3->CR1 |= USART_CR1_UE;

    // Wait until TEACK and REACK are set
    while ((USART3->ISR & USART_ISR_TEACK) == 0);
    while ((USART3->ISR & USART_ISR_REACK) == 0);

    // Enable USART3 interrupt in NVIC
    NVIC_EnableIRQ(USART3_IRQn);
}

// Interrupt handler for USART3
void USART3_IRQHandler(void) {
    if (USART3->ISR & USART_ISR_RXNE) {
        // Read the received data
        char receivedChar = USART3->RDR;

        // Add the received character to the buffer if space allows
        if (rxIndex < MAX_UART3_BUFSIZ - 1) {
            rxBuffer[rxIndex++] = receivedChar;
        }

        // Check if the received character is a newline or carriage return
        if (receivedChar == '\n' || receivedChar == '\r') {
            rxBuffer[rxIndex] = '\0';  // Null-terminate the string

            // Process the complete string (e.g., print it)
            UART3printf("Received: %s\n", rxBuffer);

            // Reset the buffer index for the next string
            rxIndex = 0;
        }
    }
}

void UART3putc(char thisChar){
    // Wait until the Transmit Data Register (TDR) is empty
    while ((USART3 -> ISR & USART_ISR_TXE) == 0);
    // Send the character by writing it to the TDR
    USART3 -> TDR = (uint8_t)thisChar;
}

void UART3puts(char *thisString) {
    while (*thisString) {
        UART3putc(*thisString++);
    }
		UART3putc('\n');
}

char UART3getc(void) {
    while ((USART3->ISR & USART_ISR_RXNE) == 0);
    return (char)(USART3->RDR);
}

void UART3printf(char *fmt, ...) {
    va_list args;
    char buffer[MAX_UART3_BUFSIZ];

    va_start(args, fmt);
    vsnprintf(buffer, MAX_UART3_BUFSIZ, fmt, args);
    va_end(args);
    
    UART3puts(buffer);
}

char UART3getcNB(void) {
    if (USART3->ISR & USART_ISR_RXNE)
        return (USART3->RDR);
    else
        return '\0';
}
*/