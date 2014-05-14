#include "tactical_IR.h"

tactical_IR::tactical_IR(uint8_t v_critical[2], uint8_t v_comparator[2], uint8_t v_analog[2])
{
  for (uint8_t i=0; i<3; i++)
  {
      critical[i] = v_critical[i];
      comparator[i] = v_comparator[i];
      analog[i] = v_analog[i];
  }
  
  for (uint8_t i=0; i<3; i++)
  {
    pinMode(comparator[i], INPUT);
    pinMode(critical[i],INPUT);
  }
}

bool tactical_IR::critical_status()
{
  //return digitalRead(critical);
}

bool tactical_IR::d_status(uint8_t number_senzor, uint8_t number_comparator, mux4067 &mux)
{
  
  mux.initial(false);		// prepnutie multiplexorov pre IR takticke senzory
  mux.pin_select(number_senzor);
  
  return digitalRead(comparator[number_comparator]);
}

float tactical_IR::a_status(uint8_t number_senzor, uint8_t number_comparator, mux4067 &mux)
{
  
  mux.initial(false);		// prepnutie multiplexorov pre IR takticke senzory
  mux.pin_select(number_senzor);
  
  return analogRead(comparator[number_comparator]);
}
