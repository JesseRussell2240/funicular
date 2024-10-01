
// PushButton SOurce File (need PC13)

#include "PushButton.h"
#include "utility.h"

void PushButton_Init(void)
{
	// Enable GPIO Port C
	// RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	ENABLE_CLOCKx( C );
	
	// Set PC13 to INPUT mode (00)
	// GPIOC -> MODER &= ~(0x3UL << (2*13));
	MODER_SET( C, 13, MODER_IN );
	
	// Set PUPD to no pull (00 for pin 13)
	// GPIOC ->PUPDR &= ~(0x3UL << (2*13));
	PUPDR_SET( C, 13, PUPDR_NP );
	
}

uint8_t PushButton_PressCheck(void)
{
	// Check if ODR of PC13 is set.  
	// if( GPIOC -> IDR & GPIO_IDR_13 )
	if(IS_BIT_SET(C->IDR, GPIO_IDR_13) )
	//  If set, button is not pressed (Active LO)
		return 0; 
	//  If cleared, button is pressed
	else
		return 1;
}

