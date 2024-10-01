unworking updated main


//main to test USAR initilization and basic serial, print and get char functions
//Code is part of lab 1 for embeded software systems
//Jesse Russell 25 jan 2024

#include "stm32f303xe.h"
#include "SysClock.h"
#include "UART.h"
#include "Utility.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Servo.h"
#include "DriveMotor.h"

#include "LCD.h"

//char cmd[11];

int main(void){
	
//	char input[50];
  float values[50];

	int wasParked = 0;
	int isParked = 0;

	
	//initilie and update core clocks to ensure frequency
	System_Clock_Init();
	SystemCoreClockUpdate();
	
	//calls uart initiliation
	UART2_Init();
	DriveMotor_Init();
	
		// Set all servos to 0 deg  before loop
    for (int16_t i = 0; i < NUM_SERVOS; ++i) {
        Servo_SetAngle(i, 0);
    }
		
		

	for(;;){
		
		
		char receivedChar[50]  = "00000000000000000";

        // Receive a character from UART
		
     UARTgets(receivedChar, 50);
     // Echo back the received character
     UARTprintf(receivedChar, "\n");
				//UARTprintf("recived :", receivedChar);
		// Split the input string into tokens separated by commas
			char *token = strtok(receivedChar, ",");
			int i = 0;
			while (token != NULL && i < 10) {
					// Convert token to float and store it in the values array
					values[i++] = atof(token);
					// Get the next token
					token = strtok(NULL, ",");
//			}
		
				
		if (receivedChar[0] =! "X"){
				isParked = values[0];
		}

		
		//transitioning from drive to park
		if(isParked == 1 && wasParked == 0){
			Servo_Init();
			wasParked = 1;
			UARTprintf("servo");
			
			//checks if the car is transitiong from park to drive
		} else if (isParked == 0 && wasParked == 1){
			
			//update the servo and save the stored values so the arm is not jerked to 00
			DriveMotor_Init();
			wasParked = 0;
			UARTprintf("drivemotorinit");
			
		} else {
			if(isParked == 1){
				//servo code
				Servo_SetAngle(0, values[1]);
				Servo_SetAngle(1, values[2]);
				Servo_SetAngle(2, values[3]);
				Servo_SetAngle(3, values[4]);
				
				//gripper
				Servo_SetAngle(3, values[5]);
				
			}else if (isParked == 0){
				
				//forward/backword
				if(values[6] > 10){
					//drive forward
						SetMotor(motorFrontLeft, DIR_FWD, values[6]);
						SetMotor(motorFrontRight, DIR_FWD, values[6] );
						SetMotor(motorRearLeft, DIR_FWD, values[6]);
						SetMotor(motorRearRight, DIR_FWD, values[6] );
					
				} else if (values[6] < 10){
					//drive Backward
						SetMotor(motorFrontLeft, DIR_BWD, values[6]);
						SetMotor(motorFrontRight, DIR_BWD, values[6] );
						SetMotor(motorRearLeft, DIR_BWD, values[6]);
						SetMotor(motorRearRight, DIR_BWD, values[6] );
				}
				
				//clock 
				if(values[7] > 10){
					//clock LEFT
						SetMotor(motorFrontLeft, DIR_BWD, values[7]);
						SetMotor(motorFrontRight, DIR_FWD, values[7] );
						SetMotor(motorRearLeft, DIR_BWD, values[7]);
						SetMotor(motorRearRight, DIR_FWD, values[7] );
					
				} else if (values[7] < 10){
					//clock RIGHT
						SetMotor(motorFrontLeft, DIR_FWD, values[7]);
						SetMotor(motorFrontRight, DIR_BWD, values[7] );
						SetMotor(motorRearLeft, DIR_FWD, values[7]);
						SetMotor(motorRearRight, DIR_BWD, values[7] );
				}
				
				//move righ
				if(values[8] > 10){
					//drive LEFT
						SetMotor(motorFrontLeft, DIR_BWD, values[8]);
						SetMotor(motorFrontRight, DIR_FWD, values[8] );
						SetMotor(motorRearLeft, DIR_FWD, values[8]);
						SetMotor(motorRearRight, DIR_BWD, values[8] );
					
				} else if (values[8] < 10){
					//Move left
						SetMotor(motorFrontLeft, DIR_FWD, values[8]);
						SetMotor(motorFrontRight, DIR_BWD, values[8] );
						SetMotor(motorRearLeft, DIR_BWD, values[8]);
						SetMotor(motorRearRight, DIR_FWD, values[8] );
				}
				
				
				
			
				
			}
			
			
		}
		
		
		
		
		//LCDprintf("The est");
			//
		
	
		
//		char receivedChar[50]  = "0000000000";

//        // Receive a character from UART
//		
//        UARTgets(receivedChar, 50);
//				//UARTprintf("\nrecived :", receivedChar);

//        // Echo back the received character
//        UARTprintf(receivedChar, "\n");
//				//UARTprintf("recived :", receivedChar);
//		// Split the input string into tokens separated by commas
//			char *token = strtok(receivedChar, ",");
//			int i = 0;
//			while (token != NULL && i < 4) {
//					// Convert token to float and store it in the values array
//					values[i++] = atof(token);
//					// Get the next token
//					token = strtok(NULL, ",");
//			}
//		
//		Servo_SetAngle(0, values[0]);
//		Servo_SetAngle(1, values[1]);
//		Servo_SetAngle(2, values[2]);
//		Servo_SetAngle(3, values[3]);
//	
		Delay_ms(900);



		
	}
		
	
	return 0;
}
}