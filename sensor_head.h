#ifndef _sensor_head_h
#define _sensor_head_h

#include "premenne.h"
#include "ultrasonic.h"
#include "proximity.h"
#include "drv8805_.h"


float head_distance(ultrasonic uz, uint8_t sharp);
void head_reset_step(motor_control *motor); 
void head_reset_step(motor_control *motor_f,motor_control *motor_b);
uint16_t rotate_head_angle(uint8_t motor, float uhol, uint8_t smer);
uint16_t rotate_head_angle( float uhol, uint8_t smer);
void head_center(motor_control motor,motor_wheel wheel, uint8_t smer);
void head_center(motor_control motor_f,motor_control motor_b,motor_wheel wheel, uint8_t smer);

#endif
