//Hayden Foster & Jesse Russell
// Motor Encoder Module Source File
/*
#include "UART.h"
#include "Encoder.h"

static uint32_t lastChannelA = 0;
static uint32_t lastChannelB = 0;
static volatile int8_t direction = 0; // 1 for forward, -1 for backward, 0 for stationary
static volatile uint32_t currentChannelA = 0; // Current state of Channel A
static volatile uint32_t currentChannelB = 0; // Current state of Channel B



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

// Declare currentChannelA and currentChannelB globally


void TIM2_IRQHandler(void)
{
    // Check if channel A triggered the interrupt
    if ((TIM2->SR) & (TIM_SR_CC1IF)) {
        // Clear the interrupt flag
        CLEAR_BITS(TIM2->SR, TIM_SR_CC1IF);

        // Read the current state of the channels
        currentChannelA = (GPIOA->IDR & (1 << 0)) >> 0; // Read state of PA0
        currentChannelB = (GPIOA->IDR & (1 << 1)) >> 1; // Read state of PA1

        // Determine direction based on the current state of channels
        if (currentChannelA == 1 && currentChannelB == 0) {
            direction = 1; // Moving forward
        } else if (currentChannelA == 0 && currentChannelB == 1) {
            direction = -1; // Moving backward
        }

        // Optionally update lastChannel states here if needed
        lastChannelA = currentChannelA;
        lastChannelB = currentChannelB; 
    }

    // Check if channel B triggered the interrupt
    if ((TIM2->SR) & (TIM_SR_CC2IF)) {
        // Clear the interrupt flag
        CLEAR_BITS(TIM2->SR, TIM_SR_CC2IF);

        // Read the current state of the channels
        currentChannelA = (GPIOA->IDR & (1 << 0)) >> 0; // Read state of PA0
        currentChannelB = (GPIOA->IDR & (1 << 1)) >> 1; // Read state of PA1

        // Determine direction based on the current state of channels
        if (currentChannelB == 1 && currentChannelA == 0) {
            direction = -1; // Moving backward
        } else if (currentChannelB == 0 && currentChannelA == 1) {
            direction = 1; // Moving forward
        }

        // Optionally update lastChannel states here if needed
        lastChannelB = currentChannelB; 
        lastChannelA = currentChannelA; 
    }

    encoderDelta();
}


// You can add some helper functions below to get the encoder period
//   ** NOTE: LEAVE ENCODER PERIOD IN INTEGER COUNT!  DON'T PERFORM FLOAT CALCULATION! (Why?)


void encoderDelta(void){
	
if (direction ==1){
	UARTprintf("Left\n");
}
else if (direction ==-1){
	UARTprintf("Right\n");
}
}
*/