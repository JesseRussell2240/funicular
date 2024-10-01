
#ifndef STEPPER_H
#define STEPPER_H
#define STEP_STOP 0
#define STEP_FULL_FORWARD 2
#define STEP_HALF_FORWARD 1
#define STEP_FULL_BACKWARD -2
#define STEP_HALF_BACKWARD -1

void Stepper_init(void);
void Update_Stepper(int stepType);

#endif