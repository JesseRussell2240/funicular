
//Ultrasonic Implementation
//Hayden & Jesse

#include "UltraSonic.h"
#include "stm32f303xe.h"
#include "utility.h"


static uint32_t Global_UltraEcho;

void Ultra_InitTrigger(void){
    // Configure GPIO Pin
    ENABLE_CLOCKx(A); 
    MODER_SET(A, 12, MODER_AF); // Set MODE to AF
    GPIO_AFRH(A, 4, 1UL); // TIM16 CH1
    GPIO_OTYPER_SET(A, 12, OTYPER_PP); // Set OTYPE to Push-pull
    PUPDR_SET(A, 12, PUPDR_NP); // Set PUPD to No pull-up/pull-down

    // Configure TIM16 CH1
    SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM16EN); // Turn on TIM16
    //SET_BITS(TIM16->PSC, 71UL); // Set PSC so it counts in 1us (1-10us)
    // Timer Period = (Prescaler + 1) / SystemClockFreq
    // 1us = (Prescaler + 1) / 72MHz
    // (Prescaler + 1) = 72
    // Prescaler = 71
    TIM16->PSC = (SystemCoreClock / 1000000) - 1;
		//TIM16
	
		TIM16->ARR = 100000 - 1; // Set ARR to 100ms (100-500ms)
    // ARR = Repeating Counter Period - 1
    SET_BITS(TIM16->CR1, TIM_CR1_ARPE); // Enable ARR preload (ARPE) in CR1
    SET_BITS(TIM16->BDTR, TIM_BDTR_MOE); // Set main output enabled (MOE) in BDTR

    // Configure TIM16 CH1 for OCM with PWM
		CLEAR_BITS(TIM16->CCMR1, TIM_CCMR1_OC1M);
		SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1M_1);
		SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1M_2);
    //SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1M); // Set TIM16 to PWM mode
    SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1PE); // Enable Output Compare Preload (OC1PE)
    SET_BITS(TIM16->CCER, TIM_CCER_CC1E); // Enable Regular Output Channel for CH1
    CLEAR_BITS(TIM16->CCER, TIM_CCER_CC1P); // Make CH1 active HI
    TIM16->CCR1 = 10; // Set CH1 CCR1 initial output waveform on-time to 10us PWM pulse width

    // Configure TIM16 CH1 for PWM (repeating mode)
   // SET_BITS(TIM16->CR1, TIM_CR1_OPM); // Set TIM16 to one-shot mode
    SET_BITS(TIM16->EGR, TIM_EGR_UG); // Force an update event to preload all the registers

		SET_BITS(TIM16->CR1, TIM_CR1_CEN); // Enable TIM16
		
}



void Ultra_InitEcho(void){
    // Configure GPIO Pin
		ENABLE_CLOCKx(C);
		//ENABLE_GPIO_CLOCK(C);
    //ENABLE_GPIO_CLOCK(C); // Enable GPIO Port C
    MODER_SET(C, 7, MODER_AF); // Set MODE to AF
    GPIO_AFR_SET(C, 7, 2); // TIM3 CH2
    PUPDR_SET(C, 7, PUPDR_NP); // Set PUPD to No pull-up/pull-down

    // Configure TIM3 CH2 (TI2)
    SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); // Turn on clock for TIM3
  //  SET_BITS(TIM3->PSC, 71UL); // Set PSC so it counts in 1us
		TIM3->PSC = (SystemCoreClock / 1000000) - 1;
     //1us = (Prescaler + 1) / 72MHz
    // (Prescaler + 1) = 72
    // Prescaler = 71
    CLEAR_BITS(TIM3->CR1, TIM_CR1_DIR); // Set TIM3 counting direction to upcounting
    //FORCE_BITS(TIM3->ARR, 0xFFFFUL, 0xFFFFUL); // Set ARR to max value
		TIM3->ARR = 0xFFFFUL;
    CLEAR_BITS(TIM3->CCMR1, TIM_CCMR1_IC2F); // Set TIM3 TI2 to No Input Filtering
    CLEAR_BITS(TIM3->CCMR1, TIM_CCMR1_IC2PSC); // Set TI2 Prescaler to zero to capture every valid transition event

    // Configure TIM3 CCR1 for PW measurement on TI2
		
		//clear needs added here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SET_BITS(TIM3->CCMR1, TIM_CCMR1_CC1S_1); // Internally connect TIM3 TI2 to CCR1
    SET_BITS(TIM3->CCER, TIM_CCER_CC1P); // Set TIM3 CCR1 to capture TI2 falling edge
    CLEAR_BITS(TIM3->CCER, TIM_CCER_CC1NP); // Set TIM3 CCR1 to capture TI2 falling edge

    // Configure TIM3 to Slave Reset on TI2 Rising Edge
    SET_BITS(TIM3->SMCR, 6UL << TIM_SMCR_TS_Pos); // Select Filtered Input Timer 2 (TI2) as the trigger source of the Timer Reset
    SET_BITS(TIM3->SMCR, 4UL << TIM_SMCR_SMS_Pos); // Select RESET slave mode on TI2 Rising Edge

    // Enable Counter Capture
    SET_BITS(TIM3->CCER, TIM_CCER_CC1E);
    SET_BITS(TIM3->CR1, TIM_CR1_CEN); // Enable TIM3 main counter
		
			//SET_BITS(TIM16->CR1, TIM_CR1_CEN); // Enable TIM16
		
}



void Ultra_Init(void){
    Ultra_InitTrigger();
    Ultra_InitEcho();
}


void Ultra_StartTrigger(void){
    SET_BITS(TIM16->CR1, TIM_CR1_CEN); // Enable TIM16
}



uint32_t Ultra_ReadSensor(void) {
	Ultra_EchoRx();
    return Global_UltraEcho / 59;  // Equation from ESS W7 slides (#6)
}

// Function to check whether echo has been received
uint8_t Ultra_EchoRx(void) {
    // Check whether (CC1IF) in SR is set
    if ((TIM3->SR) & (TIM_SR_CC1IF)) {
        Global_UltraEcho = TIM3->CCR1;  // Record TIM3 CCR1 value in a global variable for further processing
        return 1;
    }
    return 0;
}
