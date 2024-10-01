
// PushButton Header File

#ifndef __PUSHBUTTON_H
#define __PUSHBUTTON_H

#include "stm32f303xe.h"
#include "utility.h"

void PushButton_Init(void);

uint8_t PushButton_PressCheck(void);

#endif
