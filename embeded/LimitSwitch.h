#include "stm32f303xe.h"
#include "utility.h"
#include "UART3.h"

void EXTI9_5_IRQHandler(void);
void LimitSwitch_Init(void);
void SysTick_Handler(void);
extern volatile uint8_t limitSwitchTriggered;

extern volatile uint32_t millisCounter;     
extern volatile uint32_t lastTriggerTime; 
