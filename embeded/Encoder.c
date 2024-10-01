//Hayden Foster & Jesse Russell
// Motor Encoder Module Source File

#include "Encoder.h"
	static uint32_t leftEncOld = 0;
	static uint32_t leftEncNew = 0;
	static uint32_t rightEncOld = 0;
	static uint32_t rightEncNew = 0;
	
	uint32_t leftEncoderPeriod = 0;
	uint32_t rightEncoderPeriod = 0;


void Encoder_Init(void)
{
	// Enable GPIO Port A	
	 ENABLE_CLOCKx(A);
		// PA0 for Input Capture on Left Wheel
	 MODER_SET(A, 0, MODER_AF);
	 PUPDR_SET(A, 0, PUPDR_NP); 
	 GPIO_AFR_SET(A, 0, 1);
		// PA1 for Input Capture on Right Wheel
	 MODER_SET(A, 1, MODER_AF);
   PUPDR_SET(A, 1, PUPDR_NP);
	 GPIO_AFR_SET(A, 1, 1);
   
	
	// Configure TIM2 for Both CH1 and CH2 inputs
	SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	
	// Enable TIM2 on APB1
	// Set Prescaler to 1us
	 TIM2->PSC = SystemCoreClock / 1000000 - 1;
	
	// Set counting direction to upcounting
	CLEAR_BITS(TIM2->CR1, TIM_CR1_DIR);
	
	
	
	
	// Configure TIM2 CH1 for input capture on Left Encoder
	
	// Select input capture mode for CH1 (normal mode  0%01)
	//   By setting CC1S to 0%01 in CCMR1
	SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC1S_0);
	
	// ENABLE_GPIO_CLOCK input capture for CH1
	//   BY setting CC1E in CCER
	SET_BITS(TIM2->CCER, TIM_CCER_CC1E);
	
	// Detect rising edges (by clearing both input capture mode bits)
	//	 Clear both CC1P and CC1NP in CCER
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC1P);
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC1NP);
	
	// Clear CCR1 of any garbage values by setting CCR1 = 0
	CLEAR_BITS(TIM2->CCR1, TIM_CCR1_CCR1);
	
	
	
	// Configure TIM2 CH2 for input capture on Right Encoder
	
	// Select input capture mode for CH2 (normal mode  0%01)
		// BY setting CC2S to 0%01
	SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC2S_0);
	
	// ENABLE_GPIO_CLOCK input capture for CH2
		// By setting CC2E in CCER
	SET_BITS(TIM2->CCER, TIM_CCER_CC2E);
		
	// Detect rising edges (by clearing both input capture mode bits)
		// By clearing both CC2P ad CC2NP in CCER
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC2P);
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC2NP);
	
	// Clear CCR2 of any garbage values by setting CCR2 = 0
	CLEAR_BITS(TIM2->CCR2, TIM_CCR2_CCR2);
	
	
	
	
	// Configure TIM2 to generate interrupts and configure NVIC to respond
	
	// Enable Encoder channels to trigger IRQ
		// BY setting CC1IE and CC2IE in DIER
	SET_BITS(TIM2->DIER, TIM_DIER_CC1IE); 
  SET_BITS(TIM2->DIER, TIM_DIER_CC2IE);
	
	// Enable Timer 2 IRQ (TIM2_IRQn) in NVIC and set its priority to any... 
	NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM2_IRQn, ENCODER_PRIORITY); 
	 
	 
	 
	
	// Start TIM2 CH1 and CH2 Input Captures
	
	// 1. Force an update event to preload all the registers
	//		By setting UG in EGR
	 SET_BITS(TIM2->EGR, TIM_EGR_UG); 
	 
	// 2. Enable TIM2 to start counting
	//		By setting CEN in CR1
	 SET_BITS( TIM2 -> CR1, TIM_CR1_CEN );
}


// You will likely need four uint32_t global variables
//	Two for holding the calculated encoder periods for left and right wheels
	// These two variables will have to be declared static (or extern if required).  [Why?]
	
//  Two for holding the MOST RECENT HISTORICAL left and right encoder time stamps

// So that....

void TIM2_IRQHandler( void )
{

	// In TIM2 IRQ handler
	
	// Check which channel (CCR1 or CCR2) fired the interrupt by checking
	//    If CCR1 (Left Wheel) triggers the IRQ, CC1IF in SR will be set
	if((TIM2->SR) & (TIM_SR_CC1IF)){
		
			leftEncOld = leftEncNew;
			leftEncNew = TIM2->CCR1;
	
    }

	//    IF CCR2 (Right Wheel) triggers the IRQ, CC2IF in SR will be set
	if((TIM2->SR) & (TIM_SR_CC2IF)){
		
		rightEncOld = rightEncNew;
		rightEncNew = TIM2->CCR2;
    

	}
 

	// Then, in each respective case, read the CCR value as the CURRENT TIMESTAMP
	// 	1. Encooder Period = CURRENT TIMESTAMP - MOST RECENT HISTORICAL
	//  2. Update MOST RECENT HISTORICAL = CURRENT TIMESTAMP
	
	
	// [Question for you]:  What happens if timer counter overflows?
	//			i.e. takes too long for the vane to pass through the optical assembly?
	
	//  We will answer this at the end of the lab session
	
}

// You can add some helper functions below to get the encoder period
//   ** NOTE: LEAVE ENCODER PERIOD IN INTEGER COUNT!  DON'T PERFORM FLOAT CALCULATION! (Why?)


void encoderDelta(void){
	//leftEncoderPeriod = (((lNew - lOld)*28)/60000000);
	//rightEncoderPeriod = (((rNew - rOld)*28)/60000000);
	leftEncoderPeriod = (leftEncNew - leftEncOld);
	rightEncoderPeriod = (rightEncNew - rightEncOld);
}
