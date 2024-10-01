
#include "utility.h"
#include "keypad.h"
#include "PushButton.h"
#include "UART.h"

static int rowNum;
static int colNum;

//[rowNum][colNum]
static char charTable[4][4] = { {'1', '2', '3', 'A'},\
																{'4', '5', '6', 'B'},\
																{'7', '8', '9', 'C'},\
																{'*', '0', '#', 'D'}};
void keypad_Init(void)
{
	///MODER Init
	MODER_SET(B,0,MODER_GP);
	MODER_SET(B,1,MODER_GP);
	MODER_SET(B,2,MODER_GP);
	MODER_SET(B,3,MODER_GP);
	MODER_SET(B,4,MODER_IN);
	MODER_SET(B,5,MODER_IN);
	MODER_SET(B,6,MODER_IN);
	MODER_SET(B,7,MODER_IN);
	
	//PUPDR Init
	PUPDR_SET(B,0,PUPDR_NP);
	PUPDR_SET(B,1,PUPDR_NP);
	PUPDR_SET(B,2,PUPDR_NP);
	PUPDR_SET(B,3,PUPDR_NP);
	PUPDR_SET(B,4,PUPDR_PU);
	PUPDR_SET(B,5,PUPDR_PU);
	PUPDR_SET(B,6,PUPDR_PU);
	PUPDR_SET(B,7,PUPDR_PU);
	
	//OTYPER Init
	GPIO_OTYPER_SET(B,0,OTYPER_PP);
	GPIO_OTYPER_SET(B,1,OTYPER_PP);
	GPIO_OTYPER_SET(B,2,OTYPER_PP);
	GPIO_OTYPER_SET(B,3,OTYPER_PP);
	GPIO_OTYPER_SET(B,4,OTYPER_PP);
	GPIO_OTYPER_SET(B,5,OTYPER_PP);
	GPIO_OTYPER_SET(B,6,OTYPER_PP);
	GPIO_OTYPER_SET(B,7,OTYPER_PP);
}

char scanKey(void)
{
// 1. Set R1 - R4 0 (write 0x0 into PB0-3)
	CLEAR_BITS( GPIOB->ODR, GPIO_ODR_0);
	CLEAR_BITS( GPIOB->ODR, GPIO_ODR_1);
	CLEAR_BITS( GPIOB->ODR, GPIO_ODR_2);
	CLEAR_BITS( GPIOB->ODR, GPIO_ODR_3);
	
    // 1.1 reset the R-C Record so both are 0
	rowNum = 0;
	colNum = 0;

	
    // 2. Read C1-C4 (read upper nybble of PB4 - 7) 
	  // 2.1 If all 1 (0xF), no key has been pressed
	
    //   Return '\0' (NULL character)
    //   leave the function
	if (IS_BIT_SET(B->IDR, GPIO_IDR_4) 
		&& IS_BIT_SET(B->IDR, GPIO_IDR_5)
		&& IS_BIT_SET(B->IDR, GPIO_IDR_6) 
		&& IS_BIT_SET(B->IDR, GPIO_IDR_7))
	{
		return('\0');
	}
		
	//checks which IDR is set low, AKA which column is pressed
	if (IS_BIT_SET(B->IDR, GPIO_IDR_4) == 0)
	{
		colNum = 0;
	}
	else if (IS_BIT_SET(B->IDR, GPIO_IDR_5) == 0)
	{
		colNum = 1;
	}
	else if (IS_BIT_SET(B->IDR, GPIO_IDR_6) == 0)
	{
		colNum = 2;
	}
	else if (IS_BIT_SET(B->IDR, GPIO_IDR_7) == 0)
	{
		colNum = 3;
	}

	for (int i = 0; i < 4; i++) {
//shifts the ODR from 0-4
	 GPIOB->ODR |= (1U << i);
//checks if all of the IDRs are high
	if (IS_BIT_SET(B->IDR, GPIO_IDR_4)
		&& IS_BIT_SET(B->IDR, GPIO_IDR_5)
		&& IS_BIT_SET(B->IDR, GPIO_IDR_6)
		&& IS_BIT_SET(B->IDR, GPIO_IDR_7))	
	{
//returns i as the row number
			rowNum = i;
			break;
	}
}
	
    // start row scaning
	
    // 3. Debounce (call debounce())
	debounce();

   
    // colNum and rowNum locked in
 char pressedKey = charTable[rowNum][colNum];

	UARTprintf("\n\n==========\nRow: %d\n", rowNum);
	UARTprintf("Col: %d\n", colNum);
	UARTprintf("%c was pressed\n==========\n", pressedKey);

   	// 5. Use the two values to zero in on what key is being pressed
		return charTable[rowNum][colNum];
	}

	void debounce(void)
{
    // Option 1 - Wait the bounce out
    Delay_ms(200); // from utility.c

    // Option 2 - Count the bounce out
    // Up to you

}

int KeyPad_PressCheck(void)
{
    // Check if ODR of PC13 is set.
    // if( GPIOC -> IDR & GPIO_IDR_13 )
    if( IS_BIT_SET(B->IDR, GPIO_IDR_4) == 1 || IS_BIT_SET(B->IDR, GPIO_IDR_5) == 1 || IS_BIT_SET(B->IDR, GPIO_IDR_6) == 1 || IS_BIT_SET(B->IDR, GPIO_IDR_7) == 1)
    //  Button is not pressed
        return 1; 
    //  Button is pressed
    else
        return 0;
}

// Lab 4
// 1. Init PB0-7 for matrix keypad
// 2. Can scan correctly for keypress
// 3. Debounce working
// 4. Print the pressed character on terminal

// Above and Beyond
// 1. Use keypad as alternative control for stepper and servo motors
// 2. Counter-based debounce mechanism



