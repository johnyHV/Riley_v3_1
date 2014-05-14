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
float senzor_check(motor_control motor_l, motor_control motor_r, motor_wheel wheel, uint8_t smer);
void otocenie_robota(motor_control motor_l, motor_control motor_r, motor_wheel wheel, float uhol, uint8_t smer);
void prekazka(motor_control motor_l, motor_control motor_r, motor_control motor_f, motor_control motor_b, motor_wheel wheel);
void vzdialenost(motor_control motor_l, motor_control motor_r, motor_wheel wheel, motor_control motor_f, motor_control motor_b,uint8_t smer);

#endif
