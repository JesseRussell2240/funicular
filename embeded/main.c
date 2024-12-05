#include "stm32f303xe.h"
#include "SysClock.h"
#include <stdio.h>
#include "UART3.h"
#include "DriveMotor.h"
#include "LimitSwitch.h"
#include "Encoder.h"
#include "Homing.h"
#include "Auto_Driving.h"



#define MAX_UART3_BUFSIZ 60



extern char rxBuffer[MAX_UART3_BUFSIZ];




int main(void){
	
	
	
	
	 System_Clock_Init(); // Call the system clock initialization from SysClock.c
   SystemCoreClockUpdate(); // Update the system core clock variable
   
	 DriveMotor_Init(); //motor initialization
	 StopAllMotors(); //stop all motors for start
   LimitSwitch_Init(); //Limit Switch init
 	 Encoder_Init(); //Encoder init
	 homing_Init(); //Home the steering rack
	
	 UART3_Init();

		
			
   for(;;){
		 

			
			
			
		
	
	
	}

}


