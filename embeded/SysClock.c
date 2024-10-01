#include "SysClock.h"


//***********************************************************************************************
// Shell code for Nucleo-64 STM32F303RE @ 72 MHz
//
// Initialize the system clocks as follows:
//		- use the external 8 MHz clock
//		- use the PLL to multiply 9 for 72 MHz system clock
//		- use the 72 MHz clock for core and all peripheral busses
//
// WPS 30 Apr 2020 - from Zhu Virtual Com Port demo code
//		- use HSE instead of HSI
//		- 72 MHz for F303 vs 80 MHz for L476
//
//***********************************************************************************************

void System_Clock_Init(void){

	// To correctly read data from FLASH memory, the number of wait states (LATENCY)
  // must be correctly programmed according to the frequency of the CPU clock
  // (HCLK) and the supply voltage of the device.		
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |=  FLASH_ACR_LATENCY_2;
		
	// Enable the External High Speed oscillator (HSE)
	RCC->CR |= RCC_CR_HSEON;
	while((RCC->CR & RCC_CR_HSERDY) == 0);
	
	// Turn PLL off (might already be on)
	RCC->CR    &= ~RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);
	
	// Select HSE as clock source to PLL
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV; // select HSE as PLL input
	
	// Set PLL to multiply HSE by 9 to get 72 MHz
	RCC->CFGR |= RCC_CFGR_PLLNODIV; //dont divide PLL output by 2
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMUL) | RCC_CFGR_PLLMUL9; // PLL multiplier value

	// Turn PLL on and wait for it to be stable
	RCC->CR   |= RCC_CR_PLLON; 
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	// Configure System Clock to use PLL
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; // 00: MSI, 01:HSI, 10: HSE, 11: PLL
	
	// Wait until the System Clock has switched over to the PLL
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// Configure the peripheral clocks
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB (GPIO) set to 72 MHz - System Clock  (clear all bits = no prescaler)
	RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB2 (high-speed peripherals) - 72 MHz (clear all bits = no prescaler)
	RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB1 (low-speed peripherals) - 36 MHz max - divide HCLK by 2
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;		// pre-clear all bits and then set for div by 2

} // System_Clock_Init()

