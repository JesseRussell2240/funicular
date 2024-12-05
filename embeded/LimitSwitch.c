#include "LimitSwitch.h"
#include "DriveMotor.h"
//#include "UART3.h"

volatile uint8_t limitSwitchTriggered = 0;

void LimitSwitch_Init(void) {
	// Enable GPIO Clock
	ENABLE_CLOCKx(C);
	// Set GPIO Mode
	MODER_SET(C,6,MODER_IN);
	MODER_SET(C,5,MODER_IN);
	
	// GPIO Push-Pull:
	PUPDR_SET(C, 6, PUPDR_PU); // Pull Up
	PUPDR_SET(C, 5, PUPDR_PU); // Pull Up
	
	EXTI->IMR |= EXTI_IMR_IM6; // Enable Interrupt
	EXTI->IMR |= EXTI_IMR_IM5; // Enable Interrupt
	
	// Connect External Line to the GPI for PC6
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI6;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PC;
	
	// Connect External Line to the GPI for PC5
	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI5;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PC;
	
	//Set Rising Edge trigger selection for PC6
	EXTI->RTSR |= EXTI_RTSR_RT6;
	//Clear Falling Edge trigger selection for PC6
	EXTI->FTSR &= ~EXTI_FTSR_FT6;
	
	//Set Rising Edge trigger selection for PC5
	EXTI->RTSR |= EXTI_RTSR_RT5;
	//Clear Falling Edge trigger selection for PC5
	EXTI->FTSR &= ~EXTI_FTSR_FT5;
	
	// Configure NVIC for EXTI events on pin 10-15
	// Set its priority to 0 (next highest to NMIs)
	NVIC_SetPriority( EXTI9_5_IRQn, 0);
	NVIC_EnableIRQ( EXTI9_5_IRQn );
	
}

void EXTI9_5_IRQHandler(void) {

	// STOP STEPPER
	UART3printf("Limit Interupt");
	
  SetMotor(motorTurning, DIR_STOP, 0 );
  limitSwitchTriggered = 1;
	// Cleared flag by writing 1
	
	EXTI->PR |= EXTI_PR_PIF6;
	EXTI->PR |= EXTI_PR_PIF5;
	
	
	
}


