#include "stm32f303xe.h"
#include "SysClock.h"
#include "UART.h"
#include <stdio.h>
#include "utility.h"
static char cmd[16];


int main(void){

	 System_Clock_Init(); // Call the system clock initialization from SysClock.c
   SystemCoreClockUpdate(); // Update the system core clock variable
  
	 UART2_Init(); // UART2 initialization routine
	 

	
for(;;){
		 
char receivedChar = UARTgetc(); 
	// Blocking call until a character is received
	// Echo the received character back        
	UARTputc(receivedChar);                
	// Optionally, you can use UARTprintf to print formatted messages 
	UARTprintf("Received: %c\n", receivedChar); 
}
}
