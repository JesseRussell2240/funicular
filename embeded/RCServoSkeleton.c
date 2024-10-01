
// RC Servo Skeleton
#include "stm32f303xe.h"
#include "RCServo.h"
#include "utility.h"
#include "UART.h"

static int16_t currentAngle = 0;

void RCServo_Init(void)
{
	// Set up PB15 to Output, AF1, PP, no Pull
	// 1. Enable clock to Port B
	 ENABLE_CLOCKx(B);
	// 2. Set the MODER to AF
	MODER_SET(B,15,MODER_AF);
	// 3. Set the AFR..([0] or [1]?) to AF1 for PB15
	GPIO_AFRH(B,7,1UL);
	
	
 // AF1
	// 4. Set the OTYPER to Push-Pull
	 GPIO_OTYPER_SET(B, 15, OTYPER_PP);
	// 5. Set the PUPDR to no pull
	 FORCE_BITS(GPIOx(B)->PUPDR, GPIO_PUPDR_PUPDR15, 0);
	// Your Task - Use your MACRO in place of the bit-operation statements from the previous labs.
	
	
	
	// Configure TIM15
	// 1. Turn on TIM15 by feeding in the APB clock.  (set RCC_APB2ENR_TIM15EN in RCC->APB2ENR)
	  SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM15EN);
	// 2. Program the prescaler (PSC) to ensure TIM15 counts at 1us
	//		Timer Frequency = SystemClockFreq / (PreScaler + 1)
	//		Solve for PreScaler expression in terms of TimerFreq and SystemClockFreq
	//    Then, what is PSC with Timer Period = 1us?
	 TIM15->PSC = SystemCoreClock / 1000000 - 1;
	// 3. Set TIM15 to Upcounting (no need to do it, because TIM15 only knows upcounting...) ***OPTIONAL***
	// 4. Set the ARR to 20000 us period
	//      Repeating Counter Period (20000) = ARR + 1
	//    Then, what is ARR?
	TIM15->ARR = 20000 - 1;
	// 5. Enable TIM15 ARR Preload (ARPE flag on CR1)
	SET_BITS(TIM15->CR1, TIM_CR1_ARPE);
	// 6. Enable TIM15 main output, so to make it avaiable to the PWM OC (MOE flag on BDTR)
	SET_BITS(TIM15->BDTR, TIM_BDTR_MOE);
	
	
	// Configure CH2 of TIM15 for PWM OC mode
	// 1. Select PWM mode 1 (low-count mode) for TIM15 CH2 (OC2M flags - 2 bits - on CCMR1)
	//    ** PAY VERY CLOSE ATTENTION ON THIS ITEM **
	TIM15->CCMR1 &= ~(1UL << 12);
	TIM15->CCMR1 &= ~(1UL << 13);
	TIM15->CCMR1 &= ~(1UL << 14);
	TIM15->CCMR1 &= ~(1UL << 24);
	
	TIM15->CCMR1 |= 1UL << 13;
	TIM15->CCMR1 |= 1UL << 14;
	// 2. Enable the Output Compare Preload on CH2 (OC2PE flag on CCMR1)
	 SET_BITS(TIM15->CCMR1, TIM_CCMR1_OC2PE);
	// 3. Enable output channel 2 (CH2 with no negate) - (CC2E flag on CCER)
	SET_BITS(TIM15->CCER, TIM_CCER_CC2E);
	// 4. Make CH2 Active Hi (no negate) - (CC2P flag on CCER)
	CLEAR_BITS(TIM15->CCER, TIM_CCER_CC2P);
	// 5. Set CH2 PWM initial on-time to 0, so PWM will not output anything before preload is done (writing 0 into CCR2)
	 TIM15->CCR2 = 0;
	
	
	// Set TIM15 off!
	// 1. Force and Update Event to ensure all preload operations are done in sync! (UG flag on EGR)
	SET_BITS(TIM15->EGR, TIM_EGR_UG);
	// 2. Enable TIM15 Counting! (CEN flag on CR1)
	SET_BITS(TIM15->CR1, TIM_CR1_CEN);
	
	// If everything done correctly, you should be able to see on PB15 a PWM waveform
	//
	// There are 3 waveforms to capture
	//  1. Neutral Position (Pulse Width = 1500 us)
	//  2. -45 Deg Position (Pulse Width = 1050 us)
	//  3. +45 Deg Position (Pulse Width = 1950 us)
}



uint16_t RCServo_SetAngle(int16_t angle )
{
	
	  // 1. Convert the target angle to the corresponding target Pulse Width (1 degree angle maps to ~ 10us step)
    uint16_t targetPW = (uint16_t)(1500 + angle * 10);

    // 2. Check whether the PW has exceeded the mechanical (+45 ~ -45 degrees) & motor limit (+/- 90 degrees)
    //    and of course cap the target PW at the limits!
    if (targetPW < 600)
        targetPW = 600;
    else if (targetPW > 2400)
        targetPW = 2400;

		
		
    // 3. Write the new target PW into TIM15 CCR2
    TIM15->CCR2 = targetPW;

    // 4. return the calculated PW for printout in main()
		//Delay_ms(1000);
	//	UARTprintf("%d\n", targetPW);
    return targetPW;
}


void servoSweepUp(int16_t sweep){
	for (int16_t i = 0; i != sweep && currentAngle != 90; i++){
		 currentAngle++;
		 RCServo_SetAngle(currentAngle);
			//UARTprintf("%d: ", currentAngle);
      Delay_ms(10);
	}
}

void servoSweepDown(int16_t sweep){
	int16_t stepDown = -1;
	for (int16_t i = 0; i != sweep && currentAngle != -90; i+=stepDown){
		 currentAngle+=stepDown;
		 RCServo_SetAngle(currentAngle);
		//	UARTprintf("%d: ", currentAngle);
      Delay_ms(10);
	}
}
void servoReset(void){
	int16_t stepDown = -1;
	if (currentAngle > 0){
		for (int16_t i = 0; currentAngle != 0; i+=stepDown){
			currentAngle +=stepDown;
			 RCServo_SetAngle(currentAngle);
	//		UARTprintf("%d: ", currentAngle);
      Delay_ms(10);
		}
	}
	else if (currentAngle<0){
		for (int16_t i = 0; currentAngle != 0; i++){
			currentAngle++;
			 RCServo_SetAngle(currentAngle);
		//	UARTprintf("%d: ", currentAngle);
      Delay_ms(10);
	}
}
}
void servoFiring(void){
	servoSweepUp(180); //make sure a dart is chambered first time this is called
	Delay_ms(200);
	servoSweepDown(-180);
	Delay_ms(100);
	servoSweepUp(180); //chamber next dart
}

