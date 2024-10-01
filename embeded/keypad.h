#include "stm32f303xe.h"

void keypad_Init(void);
char scanKey(void);
void debounce(void);
int KeyPad_PressCheck(void);
