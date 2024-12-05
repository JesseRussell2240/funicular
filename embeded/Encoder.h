
// Motor Encoder Module Header File

#ifndef __MOTOR_ENCODER_H
#define __MOTOR_ENCODER_H

#include "stm32f303xe.h"
#include "utility.h"

#define ENCODER_PRIORITY	5


#define LEFT_ENCODER_CH		CCR1

#define RIGHT_ENCODER_CH	CCR2

#define LEFT_ENC	0
#define RIGHT_ENC 1


void Encoder_Init(void);
void TIM2_IRQHandler( void );



extern volatile int8_t direction;


// Helper Function for Debugging
uint32_t getEncoderCHValue( uint8_t enc );

extern uint32_t leftEncoderPeriod;
extern uint32_t rightEncoderPeriod;
// Alternatively, you can declare these static, then write "getter" helper functions


#endif
