// Drive Motor Module Source File
	///////////////////////////////////////////////////////////////////
	/////////////////////MOTOR DRIVER//////////////////////////////////
	///////////////////Hayden F & Jesse R/////////////////////////////
  //////////////////////////2024///////////////////////////////////

#include "DriveMotor.h"
#include "utility.h"
#include "UART3.h"

// Drive Motor Configuration Parameters
// - Motor Speed Control Pins:
//    Left Front Motor	            PC10 
//    Right Front Motor             PC11

// - Motor Direction Control Pins:
//    Left Front Motor Forward (A)	PA15
//    Left Front Motor Reverse (B)  PD2
//    Right Front Motor Forward (A) PC3 used to be stepper 
//    Right Front Motor Reverse (B) PC2 used to be stepper 

//    Left Rear Motor	             PC12 
//    Right Rear Motor             PC9

//    Left Rear Motor Forward (A)	 PB12
//    Left Rear Motor Reverse (B)  PC4
//    Right Rear Motor Forward (A) PC1 used to be stepper 
//    Right Rear Motor Reverse (B) PC0 used to be stepper 
//
//	  Gun Control
//    Gun Forward (A) PA5  these used to be DAC
//    Gun Backward (B) PC13 ill never need this but whatever
//
// - Direction Control Truth Table
//     STOP   FWD	RWD   UNDEFINED
// (A)  0      1     0      1     
// (B)  0      0     1      1
static uint16_t currentDutyCycle = 0;

void DriveMotor_Init(void)
{
	//CLEAR_BITS(RTC->TAFCR, RTC_TAFCR_PC13MODE);
	
	//	 CLEAR_BITS(RTC->TAFCR, RTC_TAFCR_PC14MODE);
	//	 CLEAR_BITS(RTC->TAFCR, RTC_TAFCR_PC15MODE);
	SET_BITS(RTC->TAFCR, RTC_TAFCR_PC13MODE);
		 
	
	// Enable GPIO Clock
		 ENABLE_CLOCKx(C);
     ENABLE_CLOCKx(D);
	   ENABLE_CLOCKx(B);
		 ENABLE_CLOCKx(A);
	

	///////////////////////////////////////////////////////////////////
	////////////////FRONT MOTORS INIT//////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Configure Motor Control Pins 
	//  For Direction Control, just ordinary GPIO configurations
	//    MODE = Output
		 MODER_SET(A,15,MODER_GP);
		 MODER_SET(D,2,MODER_GP);
		 MODER_SET(C,3,MODER_GP);
		 MODER_SET(C,2,MODER_GP);
	
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(A,15,OTYPER_PP);
  	GPIO_OTYPER_SET(D,2,OTYPER_PP);
	  GPIO_OTYPER_SET(C,3,OTYPER_PP);
	  GPIO_OTYPER_SET(C,2,OTYPER_PP);
	
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(A,15,PUPDR_NP);
		PUPDR_SET(D,2,PUPDR_NP);
		PUPDR_SET(C,3,PUPDR_NP);
		PUPDR_SET(C,2,PUPDR_NP);
		
	//    Initial Output Value should be set to 0 (STOP by default)
		CLEAR_BITS( GPIOA->ODR, GPIO_ODR_15);
		CLEAR_BITS( GPIOD->ODR, GPIO_ODR_2);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_3);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_2);
		
	//  For Speed Control, connect TIM8 channels to PC10 and PC11 with the following configurations
	//	  MODE = Alternative Function 4
		MODER_SET(C,10,MODER_AF);
		MODER_SET(C,11,MODER_AF);
		GPIO_AFRH(C,2,4UL);
		GPIO_AFRH(C,3,4UL);
		
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(C,10,OTYPER_PP);
  	GPIO_OTYPER_SET(C,11,OTYPER_PP);
		
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(C,10,PUPDR_NP);
		PUPDR_SET(C,11,PUPDR_NP);
		
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
		MODER_SET(C,9,MODER_AF);
		GPIO_AFRH(C,4,4UL);
		GPIO_AFRH(C,1,4UL);
		
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(C,12,OTYPER_PP);
  	GPIO_OTYPER_SET(C,9,OTYPER_PP);
		
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(C,12,PUPDR_NP);
		PUPDR_SET(C,9,PUPDR_NP);
		
		
		
	//////////////////////////////////////////////////////////////////
	////////////////Gun MOTOR INIT///////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	// Configure Motor Control Pins 
	//  For Direction Control, just ordinary GPIO configurations
	//    MODE = Output
		 MODER_SET(A,5,MODER_GP);
		 MODER_SET(C,13,MODER_GP);
	
	//    Output Type = Push Pull
		GPIO_OTYPER_SET(A,5,OTYPER_PP);
  	GPIO_OTYPER_SET(C,13,OTYPER_PP);
	 
	//    Pull-up / Pull-down = No Pull
		PUPDR_SET(A,5,PUPDR_NP);
		PUPDR_SET(C,13,PUPDR_NP);
	
	//    Initial Output Value should be set to 0 (STOP by default)
		CLEAR_BITS( GPIOA->ODR, GPIO_ODR_5);
		CLEAR_BITS( GPIOC->ODR, GPIO_ODR_13);
			
		Delay_ms(10);
		
		MODER_SET(A, 4, MODER_AF);
   
    // 3. Set the AFR to AF1 for PC0 to PC3                            
    GPIO_AFRL(A,4,2UL);
   
    // 4. Set the OTYPER to Push-Pull
    GPIO_OTYPER_SET(A, 4, OTYPER_PP);
   
    // 5. Set the PUPDR to no pull
    PUPDR_SET(A, 4, 0);
		

		
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
	////////////////Firing//// PWM////////////////////////////////////
	//////////////////////////////////////////////////////////////////




    // Configure TIM1
    // 1. Turn on TIM1 by feeding in the APB clock (set RCC_APB2ENR_TIM1EN in RCC->APB2ENR)
    SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
    // 2. Program the prescaler (PSC) to ensure TIM1 counts at 1us
    TIM3->PSC = SystemCoreClock / 1000000 - 1;
    // 3. Set TIM1 to Upcounting (no need to do it, because TIM1 only knows upcounting...) ***OPTIONAL***
                                //set bit 4 to 0 to ensure upcounting'
                                
    CLEAR_BITS(TIM3->CR1, TIM_CR1_CMS);  // Ensure upcounting
                                //set alighnment ot edge align piro to setting to upcount
    // 4. Set the ARR to 20000 us period
    TIM3->ARR = 1000 - 1;
    // 5. Enable TIM1 ARR Preload (ARPE flag on CR1)
    SET_BITS(TIM3->CR1, TIM_CR1_ARPE);
    // 6. Enable TIM1 main output, so to make it available to the PWM OC (MOE flag on BDTR)
    SET_BITS(TIM3->BDTR, TIM_BDTR_MOE);

    // Configure CH1 to CH4 of TIM1 for PWM OC mode
		// Servo 1 (TIM1_CH2)
    TIM3->CCMR1 |= (0x6 << TIM_CCMR1_OC2M_Pos);
    SET_BITS(TIM3->CCMR1, TIM_CCMR1_OC2PE);
    SET_BITS(TIM3->CCER, TIM_CCER_CC2E);
    CLEAR_BITS(TIM3->CCER, TIM_CCER_CC2P);
		CLEAR_BITS(TIM3->CCR2, TIM_CCR2_CCR2);
		
	


	
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
	////////////////CHANNEL 4/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	
// Configure CH4 (Complementary) of TIM8 for Right Wheel PWM output compare mode
// Select PWM mode 1 for TIM8 channel 4
TIM8->CCMR2 |= (0x6 << TIM_CCMR2_OC4M_Pos);    /////////////////////
                                               ////////////////////PWM Not working
// Enable output compare preload on channel 4  /////////////////////
SET_BITS(TIM8->CCMR2, TIM_CCMR2_OC4PE); 

// Enable the COMPLEMENTARY output channel (CH4N)
SET_BITS(TIM8->CCER, TIM_CCER_CC4E);

// Make CH4N active HI (standard polarity)
CLEAR_BITS(TIM8->CCER, TIM_CCER_CC4NP);

// Set the CH4N initial PWM ON-time to 0
CLEAR_BITS(TIM8->CCR4, TIM_CCR4_CCR4);


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
	
	 SET_BITS(TIM3->EGR, TIM_EGR_UG);
	 SET_BITS(TIM3->CR1, TIM_CR1_CEN);
	
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
        case motorFrontLeft:
            TIM8->CCR1 = dutyCycle;
            break;
        case motorFrontRight:
            TIM8->CCR2 = dutyCycle;
            break;
				 case motorRears:
            TIM8->CCR3 = dutyCycle;
            TIM8->CCR4 = dutyCycle;
            break;
				case motorTurning:
            TIM3->CCR2 = (dutyCycle);
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
            
            if (dir == DIR_FWD) {////////Rear Left Motor Forward
							//UART3printf("yog");
              SET_BITS(GPIOB->ODR, GPIO_ODR_12);
							CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4); 
							SET_BITS(GPIOC->ODR, GPIO_ODR_1);
							CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
							//UART3printf("yogirt");
							 
							
            } else if (dir == DIR_BWD) {////////Rear Left Motor Backward
               CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);  
							 SET_BITS(GPIOC->ODR, GPIO_ODR_4);  
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							 SET_BITS(GPIOC->ODR, GPIO_ODR_0);
							 //UART3printf("neeeheh");
							
            } else if (dir == DIR_STOP){////////Rear Left Motor Stop
               CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);
						 	 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
							// UART3printf("hello");
							
            } else {
              // CLEAR_BITS(GPIOB->ODR, GPIO_ODR_12);
						 	// CLEAR_BITS(GPIOC->ODR, GPIO_ODR_4);
							// CLEAR_BITS(GPIOC->ODR, GPIO_ODR_1);
							// CLEAR_BITS(GPIOC->ODR, GPIO_ODR_0);
						}
            break;
						
						
					case motorTurning:
            
            if (dir == DIR_RIGHT) {
               SET_BITS(GPIOA->ODR, GPIO_ODR_5);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
							
						} else if (dir == DIR_LEFT) {////////Rear Right Motor Backward
               CLEAR_BITS(GPIOA->ODR, GPIO_ODR_5);
							 SET_BITS(GPIOC->ODR, GPIO_ODR_13);

            } else if (dir == DIR_STOP){
               CLEAR_BITS(GPIOA->ODR, GPIO_ODR_5);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
							
            } else {
               CLEAR_BITS(GPIOA->ODR, GPIO_ODR_5);
							 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
            }
            break;
     //   default:
            
							// CLEAR_BITS(GPIOA->ODR, GPIO_ODR_15);
						 	// CLEAR_BITS(GPIOD->ODR, GPIO_ODR_2);
						//	 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_3);
						//	 CLEAR_BITS(GPIOC->ODR, GPIO_ODR_2);
           // break;
    }
}

void SetMotor( uint8_t motor, uint8_t dir, uint16_t dutyCycle )
{
	
//	SweepMotor(motor, dutyCycle);
	SetMotorDir( motor, dir );
	SetMotorSpeed(motor, dutyCycle );
}



void StopAllMotors()
{
	  SetMotor(motorFrontLeft, DIR_STOP, 0);
		SetMotor(motorFrontRight, DIR_STOP, 0 );
		SetMotor(motorRears, DIR_STOP, 0);
		SetMotor(motorTurning, DIR_STOP, 0);  
}
