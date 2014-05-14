#ifndef _proximity_h
#define _proximity_h

#include <Arduino.h>
#include "mux4067.h"
#include "premenne.h"

void proximity_initial(IR_proximity *, mux4067 &);
bool proximity_critical_status(IR_proximity *);
uint8_t proximity_d_status(IR_proximity *, uint8_t, mux4067 &, uint8_t number_comparator = 0);
float proximity_a_status(IR_proximity *, uint8_t, mux4067 &);
bool schody();
bool schody_back();

void sharp_init();
float sharp_distance(uint8_t addres);

#endif
