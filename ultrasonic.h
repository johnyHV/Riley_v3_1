#ifndef _ultrasonic_h_
#define _ultrasonic_h_

#include <Arduino.h>
#include "premenne.h"

void ultrasonic_initial(ultrasonic *);
float ultrasonic_distance(ultrasonic *);
float ultrasonic_distance_(ultrasonic *, uint8_t debug = 0);

#endif
