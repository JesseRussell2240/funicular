
//main to test LCD implimentation
//Code is part of lab 6 for embeded software systems
//Jesse Russell & Hayden Foster
//April 2 2024



#include "lcd.h"
#include "utility.h"
#include <stdarg.h>
#include <stdio.h>

//static char customChar[8] = {'<', '>', '|', '}', '{', ']', '[', '^'}; // Default custom character replacements



void LCD_GPIO_Init(void){
	//pa6,7,8,9,10,11,on out no pull and pushpull
	ENABLE_CLOCKx(A);
	
// Configure PA6 - PA11 
    // output mode 
    MODER_SET(A, 6, MODER_GP);
    MODER_SET(A, 7, MODER_GP);
    MODER_SET(A, 8, MODER_GP);
    MODER_SET(A, 9, MODER_GP);
    MODER_SET(A, 10, MODER_GP);
    MODER_SET(A, 11, MODER_GP);

    // no-pull
    PUPDR_SET(A, 6, PUPDR_NP);
    PUPDR_SET(A, 7, PUPDR_NP);
    PUPDR_SET(A, 8, PUPDR_NP);
    PUPDR_SET(A, 9, PUPDR_NP);
    PUPDR_SET(A, 10, PUPDR_NP);
    PUPDR_SET(A, 11, PUPDR_NP);

    // push-pull
    GPIO_OTYPER_SET(A, 6, OTYPER_PP);
    GPIO_OTYPER_SET(A, 7, OTYPER_PP);
    GPIO_OTYPER_SET(A, 8, OTYPER_PP);
    GPIO_OTYPER_SET(A, 9, OTYPER_PP);
    GPIO_OTYPER_SET(A, 10, OTYPER_PP);
    GPIO_OTYPER_SET(A, 11, OTYPER_PP);

}


void LCD_Init(void){
	LCD_GPIO_Init();
	
	CLEAR_BITS(LCD_PORT, LCD_PORT_BITS);
	
	LCD_E_LO;
	LCD_RS_IR;
	Delay_ms(10);
	

	//num 1
	//send 0x02 no wait
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;
	Delay_ms(5);
	
	//num 2
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;
	Delay_ms(1);
	
	//num3
	LCD_E_HI;
	LCD_BUS(0x03UL);
	LCD_E_LO;

	//num4
	LCD_E_HI;
	LCD_BUS(0x02UL);
	LCD_E_LO;

	
	

	LCDcmd(LCD_CMD_FUNCTIONS | LCD_FUNCTION_5X8FONT | LCD_FUNCTION_2LINES | LCD_FUNCTION_4BITBUS);
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_OFF);
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd( LCD_CMD_CLEAR ); 
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd( LCD_ENTRY_MODE | LCD_ENTRY_MOVE_CURSOR | LCD_ENTRY_INC);
	//send Display command
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd( LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON | LCD_DISPLAY_NOBLINK | LCD_DISPLAY_NOCURSOR);
	// 
//	Delay_ms(LCD_STD_CMD_DELAY);
	//LCDcmd( LCD_CMD_DISPLAY |LCD_CMD_DISPLAY_ON);
//	Delay_ms(LCD_STD_CMD_DELAY);
}


void LCD_Clear(void){
	Delay_ms(LCD_STD_CMD_DELAY);
	LCDcmd(LCD_CMD_CLEAR);
	Delay_ms(LCD_LONG_CMD_DELAY);
}


void LCD_HomeCursor(void){
	LCDcmd( LCD_CMD_HOME);
	Delay_ms(LCD_STD_CMD_DELAY);
}



void LCDcmd(uint8_t cmd){
	Delay_ms(LCD_STD_CMD_DELAY);
	
	LCD_E_LO;
	LCD_RS_IR;
	
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(cmd));
	LCD_E_LO;

	LCD_E_HI;
	
	LCD_BUS(LO_NYBBLE(cmd));
	LCD_E_LO;

	
}


void LCDdata(int data){
	Delay_ms(LCD_STD_CMD_DELAY);   
	LCD_E_LO;
	LCD_RS_DR;
   
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(data));
	LCD_E_LO;
   
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(data));
	LCD_E_LO;

}



void LCDputc(unsigned char ch ){
	switch(ch){
		case '\n':
				LCDcmd( LCD_CMD_SETDDADDR | LCD_DDRAM_ADDR_LINE2);
			break;
		case '\r':
				LCD_HomeCursor();
			break;
		default:
				LCDdata(ch);
			break;
	}
}


void LCDputs(char* str ){
	while(*str != '\0'){
		LCDputc(*str++);
	}

}


void LCDprintf(char* str, ... ){
	va_list args;
	char buffer[MAX_LCD_BUFSIZ];
   
	va_start(args, str);
	(void)vsnprintf(buffer, MAX_LCD_BUFSIZ, str, args);
	va_end(args);
	LCDputs(buffer);

}

