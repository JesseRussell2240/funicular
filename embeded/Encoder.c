// Hayden Foster & Jesse Russell
// Motor Encoder Module Source File

//#include "UART3.h"
#include "Encoder.h"

volatile int8_t direction = 0;         // 1 for forward, -1 for backward, 0 for stationary
volatile uint32_t encoder_Data = 0;    // Encoder data to track pulses

void Encoder_Init(void)
{
	  
    // Enable clocks for GPIOA and TIM2
    ENABLE_CLOCKx(A);
    SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // Enable TIM2 clock

    // Configure PA0 and PA1 as alternate function for TIM2 CH1 and CH2
    MODER_SET(A, 0, MODER_AF);         // PA0 as TIM2_CH1
    PUPDR_SET(A, 0, PUPDR_PU); 
    GPIO_AFR_SET(A, 0, 1);             // AF1 for TIM2

    MODER_SET(A, 1, MODER_AF);         // PA1 as TIM2_CH2
    PUPDR_SET(A, 1, PUPDR_PU);
    GPIO_AFR_SET(A, 1, 1);             // AF1 for TIM2
	
	
	//Init Turning Signal Pins
		MODER_SET(B,1,MODER_GP);
		MODER_SET(B,0,MODER_GP);
	
	//Output Type = Push Pull
	  GPIO_OTYPER_SET(B,1,OTYPER_PP);
	  GPIO_OTYPER_SET(B,0,OTYPER_PP);
	
	//Pull-up / Pull-down = No Pull
		PUPDR_SET(B,1,PUPDR_NP);
		PUPDR_SET(B,0,PUPDR_NP);
		
	//Initial Output Value should be set to 0 (STOP by default)
		CLEAR_BITS( GPIOB->ODR, GPIO_ODR_1);
		CLEAR_BITS( GPIOB->ODR, GPIO_ODR_0);
		
		
		

    // Configure TIM2 for encoder interface mode
    TIM2->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // Encoder mode 3 (count both edges of CH1 and CH2)
    TIM2->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0); // Set CH1 and CH2 as inputs
    TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P); // Rising edge polarity for both channels

    // Set prescaler to 1 µs (assuming SystemCoreClock is set)
    TIM2->PSC = SystemCoreClock / 1000000 - 1;

    // Enable interrupts on TIM2 for updates
    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
    NVIC_EnableIRQ(TIM2_IRQn);  // Enable TIM2 interrupt in NVIC

    // Start TIM2
    TIM2->CR1 |= TIM_CR1_CEN;
		TIM2->CNT = 0;
}

void TIM2_IRQHandler(void)
{
    // Check if an update interrupt occurred
    if (TIM2->SR & TIM_SR_UIF) {
			
		
		
			
    TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt flag

       
    }
	}
