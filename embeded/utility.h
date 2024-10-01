
#ifndef UTILITY_H
#define UTILITY_H
#include "stm32f303xe.h"


//lab 3

//CLOCK EN
//=====================================================================================================================
#define CLOCKx(port) RCC_AHBENR_GPIO ## port ## EN
#define ENABLE_CLOCKx(port) (((RCC->AHBENR)) |= (CLOCKx(port)))

#define GPIOx( port ) GPIO ## port
//   GPIOx(B) will be expanded into  GPIOB   as part of the code

// We need some Macros...

//Bit manipulation
//=====================================================================================================================
#define SET_BITS( port, mask ) ( (port) |= (mask) )
#define SET_BITS_P( port, reg, mask ) ( (GPIOx(port) -> (reg)) |= (mask) )
#define CLEAR_BITS( port, mask )  ( (port) &= (~(mask)) )
#define FLIP_BITS( port, mask )  ( (port) ^= (mask) )
#define FORCE_BITS( port, mask, value ) \
	( (port) = ( ( (port) & (~(mask)) ) | ( (value) & (mask) )) )
#define IS_BIT_SET(port, pin) ( (GPIOx(port) ) & (pin))


//OTYPER SET
//=====================================================================================================================
#define OTYPER_PP 0UL
#define OTYPER_OD 1UL
#define GPIO_OTYPER_SET( port, pin, type ) \
	FORCE_BITS( GPIOx(port) -> OTYPER, (1UL << ( (pin) * 1)), ((type) << ( (pin) * 1)) )

//MODER
//=====================================================================================================================
#define MODER_GP 1UL
#define MODER_AF 2UL
#define MODER_IN 0UL
#define MODER_SET(port,pin,type) FORCE_BITS( GPIOx(port) -> MODER, (3UL << ( (pin) * 2)), ((type) << ( (pin) * 2)) )

//AF SET
//=====================================================================================================================
#define GPIO_AFRL(port,pin,type) FORCE_BITS( GPIOx(port) -> AFR[0], (0xFUL << ( (pin) * 4)), ((type) << ( (pin) * 4)) )
#define GPIO_AFRH(port,pin,type) FORCE_BITS( GPIOx(port) -> AFR[1], (0xFUL << ( (pin) * 4)), ((type) << ( (pin) * 4)) )
#define GPIO_AFR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->AFR[pin >> 3UL]), (15UL << ((pin & 7UL) * 4)), ((mode) << ((pin & 7UL) * 4)))

// If this can be built correctly, I can do...
//  GPIO_OTYPER_SET( B, 2, OTYPER_PP )

//PUPDR
//=====================================================================================================================
#define PUPDR_NP 0UL
#define PUPDR_PU 1UL
#define PUPDR_PD 2UL
#define PUPDR_SET(port,pin,type) FORCE_BITS( GPIOx(port) -> PUPDR, (1UL << ( (pin) * 2)), ((type) << ( (pin) * 2)) )

// Nibble Selection
#define LO_NYBBLE(value) ((value) & 0x0FUL)
#define HI_NYBBLE(value) ((value >> 4UL) & 0x0FUL)

void Delay_ms(uint32_t ms);

#endif


