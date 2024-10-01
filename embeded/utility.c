
#include "utility.h"
#include "stm32f303xe.h"

void Delay_ms(uint32_t ms)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = ((SystemCoreClock/8)/1000UL)*ms;
	SysTick->VAL = 0;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)==0);
	
	SysTick->CTRL = 0;
	
}
