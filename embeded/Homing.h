#include "stm32f303xe.h"
#include "utility.h"
#include "UART3.h"

extern volatile uint32_t fullTurnValue;

void homing_Init(void);
int isLimitSwitchTriggered(void);
	
