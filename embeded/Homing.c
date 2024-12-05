#include "Homing.h"
#include "LimitSwitch.h"
#include "DriveMotor.h"
#include "Encoder.h"
//#include "UART3.h"

volatile uint32_t fullTurnValue = 0;

// Helper function to clear and check limit switch trigger
int isLimitSwitchTriggered() {
    if (limitSwitchTriggered) {
        limitSwitchTriggered = 0; // Clear the flag
        return 1; // Return true if triggered
    }
    return 0; // Not triggered
}

void homing_Init(void) {
	
    
    uint32_t halfTurnValue = 0;
   
    // Move to initial limit switch
    SetMotor(motorTurning, DIR_LEFT, 100);
    while (!isLimitSwitchTriggered()) {}
    
		EXTI->IMR &= ~EXTI_IMR_IM5;  // Disable interrupt for PC5
    EXTI->IMR &= ~EXTI_IMR_IM6;
			
    // Reset encoder and reverse direction
    TIM2->CNT = 0;
    
   
    
    SetMotor(motorTurning, DIR_RIGHT, 100);
			
    Delay_ms(200); // Allow time for motor to stabilize
    
    EXTI->IMR |= EXTI_IMR_IM5;  // Enable interrupt for PC5
    EXTI->IMR |= EXTI_IMR_IM6;
    
    // Move to opposite limit switch
    while (!isLimitSwitchTriggered()) {}
			
		EXTI->IMR &= ~EXTI_IMR_IM5;  // Disable interrupt for PC5
    EXTI->IMR &= ~EXTI_IMR_IM6;
    
    // Get full turn value from encoder
    fullTurnValue = TIM2->CNT;
		
    halfTurnValue = fullTurnValue / 2;
    
  
    SetMotor(motorTurning, DIR_LEFT, 100);
			
    
    // Wait until the motor reaches the midpoint with tolerance
    while (TIM2->CNT != (halfTurnValue)) {
        // Optionally, add a small delay or yield here if needed
    }
    //UART3printf("ENC %d",TIM2->CNT);
    // Stop motor
    SetMotor(motorTurning, DIR_STOP, 0);
		
		
	 EXTI->IMR &= ~EXTI_IMR_IM5;  // Disable Limit Switch after homing
   EXTI->IMR &= ~EXTI_IMR_IM6;
    
		
		
	}
