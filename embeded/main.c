#include "stm32f303xe.h"
#include "SysClock.h"
//#include "UART.h"
#include "UART3.h"
#include <stdio.h>
#include "utility.h"
#include "LimitSwitch.h"
static char cmd;


int main(void){

	 LimitSwitch_Init();
	 cmd = 'x';
	 System_Clock_Init(); // Call the system clock initialization from SysClock.c
   SystemCoreClockUpdate(); // Update the system core clock variable
  
	 UART3_Init(); // UART2 initialization routine
	 

	
for(;;){
	
  
	//UARTprintf("\nHello\n");
  //cmd = UARTgetcNB(); 
	// Blocking call until a character is received
	// Echo the received character back        
	//UARTprintf("\n%c\n", cmd);               
	// Optionally, you can use UARTprintf to print formatted messages 
	
}
}
