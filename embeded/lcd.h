//*
//main to test LCD implimentation
//Code is part of lab 6 for embeded software systems
//Jesse Russell & Hayden Foster
//April 2 2024

//


//#ifndef _LCD_H

#include "Utility.h"
#include "stm32f303xe.h"

//LCD mods
#define LCD_CMD_CLEAR 		0x01u
#define LCD_CMD_HOME 			0x02u
#define LCD_ENTRY_MODE 		0x04u
#define LCD_CMD_DISPLAY 	0x04u
#define LCD_CMD_CDSHIFT		0x08u
#define LCD_CMD_FUNCTIONS	0x20u
#define LCD_CMD_CGRAMADD 	0x40u
#define LCD_CMD_SETDDADDR 0x80u


//LCD cmd entry
#define LCD_ENTRY_MOVE_DISPLAY 	0x01
#define LCD_ENTRY_MOVE_CURSOR 	0x00
#define LCD_ENTRY_INC 					0x02
#define LCD_ENTRY_DEC 					0x00

//LCD display cmd
#define LCD_DISPLAY_BLINK				0x01
#define LCD_DISPLAY_NOBLINK			0x00
#define LCD_DISPLAY_CURSOR			0x00
#define LCD_DISPLAY_NOCURSOR		0x04
#define LCD_CMD_DISPLAY_ON			0x04
#define LCD_DISPLAY_OFF					0x00

#define LCD_CDSHIFT_RIGHT 			0X04
#define LCD_CDSHIFT_LEFT 				0X00
#define LCD_CDSHIFT_DISPLAY			0X08
#define LCD_CDSHIFT_CURSOR			0X00


#define LCD_FUNCTION_5X8FONT 		0x00
#define LCD_FUNCTION_2LINES	 		0x08
#define LCD_FUNCTION_1LINE		  0x00
#define LCD_FUNCTION_8BITBUS 		0x10
#define LCD_FUNCTION_4BITBUS 		0x00

#define LCD_LONG_CMD_DELAY 16
#define LCD_STD_CMD_DELAY 2

#define LCD_DDRAM_ADDR_LINE1 	0x00
#define LCD_DDRAM_ADDR_LINE2 	0x40


#define LCD_GPIO_PORT			A
#define LCD_PORT					GPIOA->ODR
#define LCD_RS_BIT 				(1UL << 6) //pa6
#define LCD_E_BIT 				(1UL << 7) //pa7
#define LCD_BUS_BIT 			(0xF << 8) //pa8 9 10 11
#define LCD_BUS_BIT_POS 	8

#define LCD_PORT_BITS 			(LCD_RS_BIT | LCD_E_BIT | LCD_BUS_BIT) //0x07e0 bit 6,7,8,9,10,,11


#define MAX_LCD_BUFSIZ 81 //80 AND A NULL


#define LCD_E_LO				CLEAR_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_E_HI				SET_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_RS_IR				CLEAR_BITS(LCD_PORT, LCD_RS_BIT)
#define LCD_RS_DR				SET_BITS(LCD_PORT, LCD_RS_BIT)
#define LCD_BUS(value) 	FORCE_BITS(LCD_PORT, LCD_BUS_BIT, (value) << LCD_BUS_BIT_POS)









void LCD_GPIO_Init(void);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_HomeCursor(void);

void LCDcmd(uint8_t cmd);
void LCDdata(int data);

void LCDputc(unsigned char ch );
void LCDputs(char* str );
void LCDprintf(char* str, ... );

//#endif
