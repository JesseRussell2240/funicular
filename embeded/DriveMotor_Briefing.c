// Drive Motor Module Source File
	///////////////////////////////////////////////////////////////////
	/////////////////////MOTOR DRIVER//////////////////////////////////
	///////////////////Hayden F & Jesse R/////////////////////////////
  //////////////////////////2024///////////////////////////////////

#include "DriveMotor.h"
#include "utility.h"
//#include "UART3.h"

// Drive Motor Configuration Parameters

//////////Turning Motor///////////////
// PC 2 +
// PC 3 -
// PC 10 PWM


//////////Rear Left///////////////////
// PB 12 +
// PC 4  -
// PC 11 PWM

//////////Rear Right
// PC 1 +
// PC 0 -
// PC 12 PWM


//
// - Direction Control Truth Table
//     STOP   FWD	RWD   UNDEFINED
// (A)  0      1     0      1     
// (B)  0      0     1      1
static uint16_t currentDutyCycle = 0;

void DriveMotor_Init(void)
{
	//CLEAR_BITS(RTC->TAFCR, RTC_TAFCR_PC13MODE);
	

	SET_BITS(RTC->TAFCR, RTC_TAFCR_PC13MODE);
		 
	
	// Enable GPIO Clock
		 ENABLE_CLOCKx(C);
	   ENABLE_CLOCKx(B);

	

	///////////////////////////////////////////////////////////////////
	////////////////Turning MOTORS INIT////////////////////////////////
	///////////////////////////////////////////////////////////////////
	
	// Configure Motor Control Pins 
	//  For Direction Control, just ordinary GPIO configurations
	//    MODE = Output
		
		 MODER_SET(C,3,MODER_GP);
		 MODER_SET(C,2,MODER_GP);
	
	//    Output Type = Push Pull

	  GPIO_OTYPER_SET(C,3,OTYPER_PP);
	  GPIO_OTYPER_SET(C,2,OTYPER_PP);
	
	//    Pull-up / Pull-down = No Pull
	
		PUPDR_SET(C,3,PUPDR_NP);
		PUPDR_SET(C,2,PUPDR_NP);
		
	//    Initial Output Value should be set to 0 (STOP by default)
		
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_3);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_2);
		
	//  For Speed Control, connect TIM8 channels to PC10 and PC11 with the following configurations
	//	  MODE = Alternative Function 4

		MODER_SET(C,10,MODER_AF);
		GPIO_AFRH(C,2,4UL);

		
	//    Output Type = Push Pull

  	GPIO_OTYPER_SET(C,10,OTYPER_PP);
		
	//    Pull-up / Pull-down = No Pull
	
		PUPDR_SET(C,10,PUPDR_NP);
		
	//  PC10 will be connected to TIM8 channel 1N (complemented output)
	//  PC11 will be connected to TIM8 channel 2N (complemented output)
	
	

		//////////////////////////////////////////////////////////////////
	////////////////REAR MOTORS INIT///////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
		
	// Configure Motor Control Pins 
	//  For Direction Control, just ordinary GPIO configurations
	//    MODE = Output
		 MODER_SET(B,12,MODER_GP);
		 MODER_SET(C,4,MODER_GP);
		 MODER_SET(C,1,MODER_GP);
		 MODER_SET(C,0,MODER_GP);
	
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(B,12,OTYPER_PP);
  	GPIO_OTYPER_SET(C,4,OTYPER_PP);
	  GPIO_OTYPER_SET(C,1,OTYPER_PP);
	  GPIO_OTYPER_SET(C,0,OTYPER_PP);
	
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(B,12,PUPDR_NP);
		PUPDR_SET(C,4,PUPDR_NP);
		PUPDR_SET(C,1,PUPDR_NP);
		PUPDR_SET(C,0,PUPDR_NP);
		
	//    Initial Output Value should be set to 0 (STOP by default)
		CLEAR_BITS( GPIOB->ODR, GPIO_ODR_12);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_4);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_1);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_0);
		
	//  For Speed Control, connect TIM8 channels to PC9 and PC12 with the following configurations
	//	  MODE = Alternative Function 4
		MODER_SET(C,12,MODER_AF);
		MODER_SET(C,11,MODER_AF);
		GPIO_AFRH(C,4,4UL);
		GPIO_AFRH(C,3,4UL);
		
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(C,12,OTYPER_PP);
  	GPIO_OTYPER_SET(C,11,OTYPER_PP);
		
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(C,12,PUPDR_NP);
		PUPDR_SET(C,11,PUPDR_NP);
		
		
		

		

		
		///////////////////////////////////////////////////////////////////
	////////////////MOTORS PWM////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	

	
	// Next, Configure Timer 8 for CH1N and CH2N - Mostly like Servo with 2 exceptions 
	// 1. Turn o Timer 8
	  SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);
		
	// 2. Program Timer 8 prescaler to count in microseconds
		//You need -1 to allow the timer start sequence (mandatory)
		 TIM8->PSC = SystemCoreClock / 1000000 - 1;
		 
	// 3. Set TIM8 to upcounting mode (MANDATORY!! because TIM8 default mode can be something else)
		// [DIFFERENT!] This is different from TIM15 in Servo Lab because TIM15 only supports upcounting
		CLEAR_BITS(TIM8->CR1, TIM_CR1_DIR);
		
	// 4. Set the TIM8 reload value to 1000 us period (Remember -1 because it's repeating)
		TIM8->ARR = 1000 - 1;
			
	// 5. Enable TIM8 ARR preload
		SET_BITS(TIM8->CR1, TIM_CR1_ARPE);
		
	// 6. Set TIM8 main output enabled
	  SET_BITS(TIM8->BDTR, TIM_BDTR_MOE);
		

		
	


	
  ///////////////////////////////////////////////////////////////////
	////////////////CHANNEL 1/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Configure CH1N (Complementary) of TIM8 for Left Wheel PWM output compare mode
	
	// 1. Select PWM mode 1 for TIM8 channel 1N
		// Set OC1M bits to 0110 (i.e. set bit 1 and 2) in CCMR1
		// Read manual again to confirm
  
	  TIM8->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos);
	 
	// 2. Enable output compare preload on channel 1 (i.e. update counter register only on timer enabled (UE) )
	    // Set OC1PE bit in CCMR1
	  SET_BITS(TIM8->CCMR1, TIM_CCMR1_OC1PE); 
	 
	// 3. Enable the COMPLEMENTARY output channel (CH1N)
		// Set CC1NE bit in CCER
		// [DIFFERENT!] This is defferent because PC10 and PC11 only has AF options of TIM8 CH1N/CH2N
		// Read Manual to confirm
	  SET_BITS(TIM8->CCER, TIM_CCER_CC1NE);
	
	// 4. Make CH1N active HI (standard polarity)
		// Clear CC1NP bit in CCER
		// [DIFFERENT!] Also because we are working with completentary output
  	CLEAR_BITS(TIM8->CCER, TIM_CCER_CC1NP);
	
	// 5. Set the CH1N initial PWM ON-time to 0 by clearing CCR1, so that PWM will not be outputted until the first 1ms elapsed
    CLEAR_BITS(TIM8->CCR1, TIM_CCR1_CCR1);
	
	///////////////////////////////////////////////////////////////////
	////////////////CHANNEL 2/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Configure CH2N (Complementary) of TIM8 for Right Wheel PWM output compare mode
	
	// 1. Select PWM mode 1 for TIM8 channel 2N
		// Set OC2M to 0110 in CCMR1 - same as CH1N configurations
		// Read Manual to confirm
	 TIM8->CCMR1 |= (0x6 << TIM_CCMR1_OC2M_Pos);
	 
	// 2. Enable output compare preload on channel 2 (i.e. update counter register only on timer enabled (UE) )
		// Set OC2PE to 1 in CCMR1 - same as CH1N configurations
	 SET_BITS(TIM8->CCMR1, TIM_CCMR1_OC2PE); 
	 
	// 3. Enable the COMPLEMENTARY output channel (CH2N)
		// Set CC2NE in CCER - same as CH1N
	 SET_BITS(TIM8->CCER, TIM_CCER_CC2NE);
	 
	// 4. Make CH2N active HI (standard polarity)
		// Clear CC2NP in CCER - same as CH1N
	 CLEAR_BITS(TIM8->CCER, TIM_CCER_CC2NP);
	
	// 5. Set the CH2N initial PWM ON-time to 0 by clearing CCR1, so that PWM will not be outputted until the first 1ms elapsed
   CLEAR_BITS(TIM8->CCR2, TIM_CCR2_CCR2);
	
  ///////////////////////////////////////////////////////////////////
	////////////////CHANNEL 3/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Select PWM mode 1 for TIM8 channel 3
TIM8->CCMR2 |= (0x6 << TIM_CCMR2_OC3M_Pos);

// Enable output compare preload on channel 3
SET_BITS(TIM8->CCMR2, TIM_CCMR2_OC3PE); 

// Enable the COMPLEMENTARY output channel (CH3N)
SET_BITS(TIM8->CCER, TIM_CCER_CC3NE);

// Make CH3N active HI (standard polarity)
CLEAR_BITS(TIM8->CCER, TIM_CCER_CC3NP);

// Set the CH3N initial PWM ON-time to 0
CLEAR_BITS(TIM8->CCR3, TIM_CCR3_CCR3);


 

	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Finally, Start TIM8 CH1N and CH2N Outputs	 
	// 1. Force an update event to preload all the registers
		// Set UG to 1 in TIM8 EGR
	 SET_BITS(TIM8->EGR, TIM_EGR_UG); 
	// 2. Enable TIM8 to start counting
		// Set CEN in TIM8 CR1
	 SET_BITS( TIM8 -> CR1, TIM_CR1_CEN );
	
	
}




void SetMotorSpeed( uint8_t motor, uint16_t dutyCycle )
{
	// This part is the same as Servo
	//		Write different values into CCR1/CCR2 to control the motor speed using dutyCycle
	//      Make sure you cap the DutyCycle to 100% / 0% so to not cause odd system behaviours
	if (dutyCycle > 100){
	dutyCycle = 100;
	}
	else if (dutyCycle < 50){
	dutyCycle = 50;
	}
	currentDutyCycle = dutyCycle;
	dutyCycle *= 10;
	
	 switch (motor) {
       
        case motorTurning:
            TIM8->CCR1 = dutyCycle;
            break;
				 case motorRears:
            TIM8->CCR2 = dutyCycle;
            TIM8->CCR3 = dutyCycle;
            break;
  }
}	






void SetMotorDir( uint8_t motor, uint8_t dir )
{
	// - Direction Control Truth Table
	//     STOP   FWD	RWD   UNDEFINED
	// (A)  0      1     0      1     
	// (B)  0      0     1      1

	// Deploy a switch structure to set the corresponding GPIO pins according to the direction input
	 switch (motor) {
						
				case motorRears:
            
            if (dir == DIR_FWD) {////////Back Wheels Motor Forward
						
              SET_BITS(GPIOB->ODR, GPIO_ODR_12);
							CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4); 
							SET_BITS(GPIOC->ODR, GPIO_ODR_1);
							CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
							
							 
							
            } else if (dir == DIR_BWD) {////////Back Wheels Motor Backward
               CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);  
							 SET_BITS(GPIOC->ODR, GPIO_ODR_4);  
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							 SET_BITS(GPIOC->ODR, GPIO_ODR_0);
							
							
            } else if (dir == DIR_STOP){////////Back Wheels Motor Stop
               CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);
						 	 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
							
							
            } else {
               CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);
						 	 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
						}
            break;
						
						
					case motorTurning:
            
            if (dir == DIR_RIGHT) {
               SET_BITS(GPIOC->ODR, GPIO_ODR_3);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_2);
							
							
						} else if (dir == DIR_LEFT) {
               CLEAR_BITS(GPIOC->ODR, GPIO_ODR_3);
							 SET_BITS(GPIOC->ODR, GPIO_ODR_2);
							
							

            } else if (dir == DIR_STOP){
               CLEAR_BITS(GPIOC->ODR, GPIO_ODR_3);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_2);
							
            } else { //Stop
               CLEAR_BITS(GPIOC->ODR, GPIO_ODR_3);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_2);
            }
            break;
    
    }
}

void SetMotor( uint8_t motor, uint8_t dir, uint16_t dutyCycle )
{
	
	SetMotorDir( motor, dir );
	SetMotorSpeed(motor, dutyCycle );
	
	if (TIM2->CNT > 33){
							
				//Left Turning Signal On
				SET_BITS(GPIOB->ODR, GPIO_ODR_1);	
				CLEAR_BITS(GPIOB->ODR, GPIO_ODR_0);
							
		}
						
		else if (TIM2->CNT < 27) {
							
				//Right Turning Signal On
				SET_BITS(GPIOB->ODR, GPIO_ODR_0);	
				CLEAR_BITS(GPIOB->ODR, GPIO_ODR_1);
							
		}
						
		else {
					CLEAR_BITS(GPIOB->ODR, GPIO_ODR_1);
					CLEAR_BITS(GPIOB->ODR, GPIO_ODR_0);
		}
}


void TurnMotor(uint16_t turningAngle )
{
	
	
	while (TIM2->CNT != (turningAngle)){
		
	if (turningAngle > TIM2->CNT){
		
	SetMotorDir( motorTurning, DIR_RIGHT );
	SetMotorSpeed(motorTurning, 100 );
	}
	else if (turningAngle < TIM2->CNT){
		
	SetMotorDir( motorTurning, DIR_LEFT );
	SetMotorSpeed(motorTurning, 100 );
	}
	else{
	SetMotor(motorTurning, DIR_STOP, 0 );
	}
	
	}
	SetMotor(motorTurning, DIR_STOP, 0 );
}


void StopAllMotors()
{
		SetMotor(motorRears, DIR_STOP, 0);
		SetMotor(motorTurning, DIR_STOP, 0);  
}
