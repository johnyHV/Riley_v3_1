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

float motor_distance(motor_control *motor);
float angle_twist(motor_control *motor, motor_wheel * wheel);
void calc_step_of_distance(float distance, motor_control *motor);
float senzor_check(uint8_t smer);
void otocenie_robota(float uhol, uint8_t smer);
void otocenie_robota_k(float uhol, uint8_t smer);
void prekazka(uint8_t smer);
void vzdialenost_stena(uint8_t smer);
void vzdialenost_stena(uint8_t smer, uint8_t vzdialenost);
bool test(uint8_t smer);
void motor_on_front();
void motor_on_front_nr();
void motor_on_back();
void motor_on_back_nr();
void motor_stop();
void back_cm(float);
void front_cm(float);
void front_head_scan();
void front_back_head_scan();
void prekazka_vzadu();
void prezentacia_stol();
void prezentacia_stol_e();

#endif
