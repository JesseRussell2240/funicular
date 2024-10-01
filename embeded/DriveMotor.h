#include "utility.h"
#include "stm32f303xe.h"

#define motorFrontLeft  0UL
#define motorFrontRight 1UL
#define motorRearLeft  2UL
#define motorRearRight 3UL
#define motorFiring  4UL

#define DIR_FWD    0UL
#define DIR_BWD    1UL
#define DIR_STOP   2UL

void DriveMotor_Init(void);
void SetMotorSpeed( uint8_t motor, uint16_t dutyCycle );
void SetMotorDir( uint8_t motor, uint8_t dir );
void SetMotor( uint8_t motor, uint8_t dir, uint16_t dutyCycle );
void SweepMotor( uint8_t motor, uint16_t dutyCycle);
