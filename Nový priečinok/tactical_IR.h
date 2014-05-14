#ifndef _tactical_IR_h
#define _tactical_IR_h

#include <Arduino.h>
#include "mux4067.h"


class tactical_IR {
  	private:
	  uint8_t comparator[2];
	  uint8_t analog[2];
          uint8_t critical[2];
	
	public:
      tactical_IR(uint8_t v_critical[2], uint8_t v_comparator[2], uint8_t v_analog[2]);
	  ~tactical_IR() {};
	  
	  bool critical_status();
      bool d_status(uint8_t number_senzor, uint8_t number_comparator, mux4067 &mux);
      float a_status(uint8_t number_senzor, uint8_t number_comparator, mux4067 &mux);
};

#endif
