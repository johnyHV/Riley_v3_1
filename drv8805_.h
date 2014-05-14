#ifndef _drv8805_h
#define _drv8805_h
    
#include <Arduino.h>
#include "mux4067.h"
#include "premenne.h"

// 0 - sm0,1-sm1
const bool motor_mod[3][2] = {{0,0},
                              {1,0},
                              {0,1}};


void motor_initial(motor_control *motor);
int motor_step(motor_control *motor_0, motor_control *motor_1, motor_wheel * wheel, bool dir_0, bool dir_1, uint8_t time=5, bool enable_0 = true, bool enable_1 = true);
int motor_step(motor_control *motor, motor_wheel * wheel, bool dir, uint8_t time=50, bool enable = true);
void motor_set_mod(motor_control *motor, uint8_t mod); 
void motor_enable(motor_control *motor,  bool enable);
void motor_enable(motor_control *motor_0, motor_control *motor_1,bool enable0, bool enable1);
void motor_reset(motor_control *motor);

#endif
 
