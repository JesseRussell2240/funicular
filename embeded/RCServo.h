
#include "stm32f303xe.h"

#ifndef RCSERVO_H
#define RCSERVO_H

void RCServo_Init(void);
uint16_t RCServo_SetAngle(int16_t angle);
void servoSweepUp(int16_t sweep);
void servoSweepDown(int16_t sweep);
void servoReset(void);
void resetFiring(void);
void servoFiring(void);

#endif