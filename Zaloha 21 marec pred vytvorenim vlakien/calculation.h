#ifndef _calculation_h
#define _calculation_h
    
#include <Arduino.h>
#include "premenne.h"
#include "ultrasonic.h"
#include "mux4067.h"
#include "proximity.h"
#include "sensor_head.h"
#include "drv8805_.h"
#include "mag_and_acc.h"

float motor_distance(motor_control *motor,motor_wheel * wheel);
float angle_twist(motor_control *motor,motor_wheel * wheel);
float senzor_check(uint8_t smer);
void otocenie_robota(float uhol, uint8_t smer);
void prekazka();
void vzdialenost_stena(uint8_t smer);

#endif
