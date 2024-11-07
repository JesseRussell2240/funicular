#include "stm32f303xe.h"
#include "SysClock.h"
//#include "UART.h"
#include <stdio.h>
#include "utility.h"
#include "LimitSwitch.h"
#include "Encoder.h"
#include "DriveMotor.h"

#define MAX_UART3_BUFSIZ 60

//static int directionFlag;

//static char cmd[60] = "2";
extern char rxBuffer[MAX_UART3_BUFSIZ];


int main(void){
	
	 System_Clock_Init(); // Call the system clock initialization from SysClock.c
   SystemCoreClockUpdate(); // Update the system core clock variable
	 

	// LimitSwitch_Init();
	 DriveMotor_Init();
	// UART3_Init(); // UART3 initialization routine
	// Encoder_Init();

	
	 //cmd = 'x';
	 //StopAllMotors(); //stop all motors for start
	 //SetMotor(motorRears, DIR_BWD, 100 );
	 //directionFlag = 0;
	
	
		SetMotor(2UL, 1UL, 100);
for(;;){
	

		/*
 	 switch(rxBuffer[0])
			{
			case '1': //Full Speed FWD
			  
				 //SetMotor(motorRears, DIR_FWD, 100);
				 
				
		  break;
			
			case '2': //Half Speed FWD
				 
				SetMotor(motorRears, DIR_FWD, 100 );
				//UART3printf("12345");
		  break;
			
			case '3': //Full Speed BWD
		
				 //SetMotor(motorRears, DIR_BWD, 100 );
				SetMotor(motorRears, DIR_BWD, 100 );
		  break;
			
			case '4': //Half Speed BWD
	
				 //SetMotor(motorRears, DIR_BWD, 60 );
				//SetMotor(motorRears, DIR_STOP, 100 );
		  break;
			
			case '0': //Stop
				
				 SetMotor(motorRears, DIR_STOP, 0 );
			   //SetMotor(motorTurning, DIR_STOP, 0);
		  break;
			
		}
	
		
	
		
//////////////////////Direction Tracker//////////////////////////////////

 //////////////////////Forward////////////////////////////////////////////   
    if ((GPIOB->ODR & GPIO_ODR_12) && !(GPIOC->ODR & GPIO_ODR_4) && 
         (GPIOC->ODR & GPIO_ODR_1) && !(GPIOC->ODR & GPIO_ODR_0)) {

        if (!directionFlag) {  // Only print if the statement wasn't printed yet
					UART3printf("Direction: Forward");
            directionFlag = 1;  // Set the flag to prevent further printing
        }
    }
//////////////////////Backward////////////////////////////////////////////
    else if (!(GPIOB->ODR & GPIO_ODR_12) && (GPIOC->ODR & GPIO_ODR_4) && 
              !(GPIOC->ODR & GPIO_ODR_1) && (GPIOC->ODR & GPIO_ODR_0)) {

        if (!directionFlag) {
            UART3printf("Direction: Backward");
            directionFlag = 1;
        }
    }
//////////////////////Stopped////////////////////////////////////////////
    else if (!(GPIOB->ODR & GPIO_ODR_12) && !(GPIOC->ODR & GPIO_ODR_4) && 
              !(GPIOC->ODR & GPIO_ODR_1) && !(GPIOC->ODR & GPIO_ODR_0)) {

        if (!directionFlag) {
            UART3printf("Direction: Stopped");
            directionFlag = 1;
        }
    }
//////////////////////Changing State////////////////////////////////////////////
    else {
        directionFlag = 0;
    }
*/
	
}

}

