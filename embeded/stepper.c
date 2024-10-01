
#include "stm32f303xe.h"
#include "stepper.h"
#include "utility.h"
#include "UART.h"

static int HSpattern[8] = {1,5,4,6,2,10,8,9};
static int count;

/*
while(1)
{
	HSpattern[x & 0000111];
	x++
}
*/

void Stepper_init(void)
{
	//enable GPIO port C clock source
RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
//set the MODER for PC0 to PC3 to GP (10)
GPIOC->MODER &= ~(0x03UL << (0*2));
GPIOC->MODER &= ~(0x03UL << (1*2));
GPIOC->MODER &= ~(0x03UL << (2*2));
GPIOC->MODER &= ~(0x03UL << (3*2));
	
GPIOC->MODER |= (0x01UL << (0*2));
GPIOC->MODER |= (0x01UL << (1*2));
GPIOC->MODER |= (0x01UL << (2*2));
GPIOC->MODER |= (0x01UL << (3*2));



// Set the output speed (OSPEED) register to low speed
// 00 - LOW, 01 - MED, 10 - FAST, 11 - HIGH
// Hint: OSPEEDR
GPIOC -> OSPEEDR &= ~(0x3UL << (0*2));  
GPIOC -> OSPEEDR &= ~(0x3UL << (1*2));    
GPIOC -> OSPEEDR &= ~(0x3UL << (2*2));   
GPIOC -> OSPEEDR &= ~(0x3UL << (3*2));    

// Set PUPDR of PC2 and PC3 to No Pull
GPIOC -> PUPDR &= ~(0x1UL << (0*2));    
GPIOC -> PUPDR &= ~(0x1UL << (1*2));    
GPIOC -> PUPDR &= ~(0x1UL << (2*2));    
GPIOC -> PUPDR &= ~(0x1UL << (3*2));   

// Set OTYPER to Push-Pull for PA2 and PA3
GPIOC -> OTYPER &= ~(0x1UL << 0);     
GPIOC -> OTYPER &= ~(0x1UL << 1);        
GPIOC -> OTYPER &= ~(0x1UL << 2);        
GPIOC -> OTYPER &= ~(0x1UL << 3);       
	
	//lab 3
	//GPIO_OTYPER_SET(C, 0, OTYPER_PP);
	//GPIO_OTYPER_SET(C, 1, OTYPER_PP);
	//GPIO_OTYPER_SET(C, 2, OTYPER_PP);
	//GPIO_OTYPER_SET(C, 3, OTYPER_PP);
}

void Update_Stepper(int StepType)
{
		 // Increment the step count based off the step type
		count += StepType;
	
 // Makes the count stays within the valid range
	if (count >= 8) {
		count = 0;
	}
	else if (count <=-1){
		count = 7;
	}
	
	switch(count)
	{
	  case 0:
			// Setting GPIOC outputs for step 0
        GPIOC->ODR |= 1UL << 0;
				GPIOC->ODR &= ~(1UL << 1);
				GPIOC->ODR &= ~(1UL << 2);
				GPIOC->ODR &= ~(1UL << 3);
			break;
				
		case 1:
			// Setting GPIOC outputs for step 1
				GPIOC->ODR |= (1UL << 0);
				GPIOC->ODR &= ~(1UL << 1);
				GPIOC->ODR |= 1UL << 2;
				GPIOC->ODR &= ~(1UL << 3);			
			break;

		case 2:
			// Setting GPIOC outputs for step 2
        GPIOC->ODR &= ~(1UL << 0);
				GPIOC->ODR &= ~(1UL << 1);
				GPIOC->ODR |= 1UL << 2;
				GPIOC->ODR &= ~(1UL << 3);	
			break;
			
		case 3:
			// Setting GPIOC outputs for step 3
        GPIOC->ODR &= ~(1UL << 0);
				GPIOC->ODR |= 1UL << 1;
				GPIOC->ODR |= 1UL << 2;
				GPIOC->ODR &= ~(1UL << 3);			
			break;
			
		case 4:
			// Setting GPIOC outputs for step 4
				GPIOC->ODR &= ~(1UL << 0);
				GPIOC->ODR |= 1UL << 1;
				GPIOC->ODR &= ~(1UL << 2);
				GPIOC->ODR &= ~(1UL << 3);		
			break;
		
		case 5:
			// Setting GPIOC outputs for step 5
        GPIOC->ODR &= ~(1UL << 0);
				GPIOC->ODR |= 1UL << 1;
				GPIOC->ODR &= ~(1UL << 2);
				GPIOC->ODR |= 1UL << 3;	
			break;
			
		case 6:
			// Setting GPIOC outputs for step 6
        GPIOC->ODR &= ~(1UL << 0);
				GPIOC->ODR &= ~(1UL << 1);
				GPIOC->ODR &= ~(1UL << 2);
				GPIOC->ODR |= 1UL << 3;	
			break;
			
		case 7:
			// Setting GPIOC outputs for step 7
				GPIOC->ODR |= 1UL << 0;
				GPIOC->ODR &= ~(1UL << 1);
				GPIOC->ODR &= ~(1UL << 2);
				GPIOC->ODR |= 1UL << 3;	
			break;
	 }
		
	 //add prints
		 UARTprintf("StepType: %d, ", StepType);
	   UARTprintf("Pattern: %d\n", HSpattern[count]);
		 //UARTprintf("");
}

