#ifndef _drv8805_h
#define _drv8805_h

#include <Arduino.h>
#include "mux4067.h"
#include "premenne.h"

// 0 - sm0,1-sm1
const bool motor_mod[3][2] = {
    {0, 0},
    {1, 0},
    {0, 1}};


void motor_initial(motor_control *);
int motor_step(motor_control *, motor_control *, motor_wheel *, bool, bool, bool enable_0 = true, uint8_t time = 5);
int motor_step(motor_control *, motor_wheel *, bool, bool enable = true, uint8_t time = 50);
void motor_set_mod(motor_control *, uint8_t);
void motor_enable(motor_control *, bool);
void motor_enable(motor_control *, motor_control *, bool, bool);
void motor_reset(motor_control *);

#endif

