#ifndef _ultrasonic_h_
#define _ultrasonic_h_

#include <Arduino.h>
#include "premenne.h"

void ultrasonic_initial(ultrasonic *data);
float ultrasonic_distance(ultrasonic *data);
float ultrasonic_distance_(ultrasonic *data, uint8_t debug=0);

#endif
