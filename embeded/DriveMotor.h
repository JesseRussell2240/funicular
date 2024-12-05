#include "utility.h"
#include "stm32f303xe.h"

#define motorFrontLeft  0UL
#define motorFrontRight 1UL
#define motorRears      2UL
#define motorTurning    3UL

#define DIR_FWD    0UL
#define DIR_BWD    1UL
#define DIR_STOP   2UL

#define DIR_LEFT   3UL
#define DIR_RIGHT  4UL

void DriveMotor_Init(void);
void SetMotorSpeed( uint8_t motor, uint16_t dutyCycle );
void SetMotorDir( uint8_t motor, uint8_t dir );
void SetMotor( uint8_t motor, uint8_t dir, uint16_t dutyCycle );
void SweepMotor( uint8_t motor, uint16_t dutyCycle);
void TurnMotor(uint16_t turningAngle );
void StopAllMotors(void);
