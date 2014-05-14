#ifndef _proximity_h
#define _proximity_h

#include <Arduino.h>
#include "mux4067.h"
#include "premenne.h"

void proximity_initial(IR_proximity *proximity, mux4067 &mux);
bool proximity_critical_status(IR_proximity *proximity);
uint8_t proximity_d_status(IR_proximity *proximity, uint8_t sensor, mux4067 &mux, uint8_t number_comparator = 0);
float proximity_a_status(IR_proximity *proximity, uint8_t sensor, mux4067 &mux, uint8_t number_comparator = 0);

void sharp_init();
float sharp_distance(uint8_t addres);

#endif
