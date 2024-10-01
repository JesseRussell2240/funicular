#ifndef __Ultrasonic_H
#define __Ultrasonic_H

#include "stm32f303xe.h"



void Ultra_InitTrigger(void);
void Ultra_InitEcho(void);
void Ultra_Init(void);
void Ultra_StartTrigger(void);
uint32_t Ultra_ReadSensor(void);
uint8_t Ultra_EchoRx(void);
	
#endif
