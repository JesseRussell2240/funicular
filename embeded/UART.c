
#include "UART.h"
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>

#define MAX_UART_BUFSIZ 60

void UART2_Init(void){
	
//Part 1: clock source config

//enable USART2 clock to enable subsystem
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

//select USART2 clock source
//00: PCLK
//01: System clock
//10: HSI16 clock source
//11: LSE clock source
RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_1; //reset bit 1
RCC->CFGR3 |= RCC_CFGR3_USART2SW_0; //set bit 0

//Part 2: GPIO config to enable PA2 and PA3 for UART2 alt functions

//enable GPIO port A clock source
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//set the MODER for PA2 and PA3 to AF (10)
GPIOA->MODER &= ~(0x03UL << (2*2));
GPIOA->MODER &= ~(0x03UL << (2*3));

GPIOA->MODER |= (0x02UL << (2*2));
GPIOA->MODER |= (0x02UL << (2*3));
	
//select USART2 as the alt function
GPIOA->AFR[0] &= ~(0x0FUL << (2*4));
GPIOA->AFR[0] &= ~(0x0FUL << (3*4));

GPIOA->AFR[0] |= (0x07UL << (2*4));
GPIOA->AFR[0] |= (0x07UL << (3*4));

//set the output speed (OSPEED) register to low speed for PA2 and PA3\
//00 - Low, 01 - MEd, 10 - Fast, 11 - High
GPIOA->OSPEEDR &= ~(0x3UL << (2*2));
GPIOA->OSPEEDR &= ~(0x3UL << (3*2));

//set PUPDR of PA2 and PA3 to No pull
GPIOA->PUPDR &= ~(0x1UL << (2*2));
GPIOA->PUPDR &= ~(0x1UL << (3*2));

//set OTYPER to push/pull for PA2 and PA3
GPIOA->OTYPER &= ~(0x1UL << 2);
GPIOA->OTYPER &= ~(0x1UL << 3);

//Part 3: configure the UART2 subsystems for 9600 BAUD com

//Disable UART2 (Write 0 into UE of USART2->CR1)
USART2->CR1 &= ~USART_CR1_UE;

//set word length (write 0 to both M1 and M0 of USART2->CR1)
USART2->CR1 &= ~USART_CR1_M;

//set oversampling rate to 16x (write 0 into OVER8 of USART2->CR1)
USART2->CR1 &= ~USART_CR1_OVER8;

//set stop bit to 1 stop bit (write 00 into USART2->CR2)
USART2->CR2 &= ~USART_CR2_STOP;

//set BAUD rate register
//BRR_Value = SystemClock / Baud Rate
//SystemCoreClock (72000000) defined in SysClock.h
//Baud Rate already defined in UART.h
//Write SystemCoreClock / BAUD_RATE into USART2->BRR
USART2->BRR = SystemCoreClock / BAUD_RATE;

//Turn on transmit/receiver blocks by writing 1 into RE and TE of USART2->CR1
USART2->CR1 |=USART_CR1_TE;
USART2->CR1 |=USART_CR1_RE;

//Re-endable USART2 by writing 1 into UE of USART2->CR1
USART2->CR1 |=USART_CR1_UE;

//wait for REACK and TEACK to turn to 1 on USART2->ISR
while((USART2->ISR & USART_ISR_TEACK)==0); //wait till transmitter enabled
while((USART2->ISR & USART_ISR_REACK)==0); //wait till receiver enabled

}

void UARTputc(char thisChar){
    // Wait until the Transmit Data Register (TDR) is empty
    while ((USART2 -> ISR & USART_ISR_TXE) == 0);
    
    // Send the character by writing it to the TDR
    USART2 -> TDR = (uint8_t)thisChar;
}

void UARTputs(char *thisString) {
    // go through the characters in the string
    while (*thisString) {
        // Send each character using UARTputc function
        UARTputc(*thisString++);
    }
}

// Function to receive a character from UART
char UARTgetc(void) {
    // Wait until the Receive Data Register (RDR) is not empty
    while ((USART2->ISR & USART_ISR_RXNE) == 0);
    
    // Return the received character from the RDR
    return (char)(USART2->RDR);
}

// Function to print data over UART
void UARTprintf(char *fmt, ...) {
    va_list args;
    //Allocatin the size of the char
    char buffer[MAX_UART_BUFSIZ];

    // Initialize the variable arguments list
    va_start(args, fmt);
    
    // Use vsnprintf to format the variable arguments into the buffer
    vsnprintf(buffer, MAX_UART_BUFSIZ, fmt, args);
    
    // Clean up the variable arguments list
    va_end(args);
    
    // Send the formatted string using UARTputs
    UARTputs(buffer);
}

char UARTgetcNB(void)
{
	if(USART2->ISR & USART_ISR_RXNE)
		return(USART2->RDR);
	else
		return('\0');
}
