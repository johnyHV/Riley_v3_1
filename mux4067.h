#ifndef _mux_4067_h_
#define _mux_4067_h_

#include <Arduino.h>

const byte pin[16][4] = {{0,0,0,0},
						{1,0,0,0},
						{0,1,0,0},
						{1,1,0,0},
						{0,0,1,0},
						{1,0,1,0},
						{0,1,1,0},
						{1,1,1,0},
						{0,0,0,1},
						{1,0,0,1},
						{0,1,0,1},
						{1,1,0,1},
						{0,0,1,1},
						{1,0,1,1},
						{0,1,1,1},
						{1,1,1,1}};

class mux4067 {
  	private:
	  uint8_t control_pin[4];
	  uint8_t init;
	  uint8_t outp;
	
	public:
	  mux4067(uint8_t (&v_control_pin)[4], uint8_t v_initial, uint8_t v_output); // posledny pin, je pin s najnizsiou vahou
	  ~mux4067() {};
	  
	  void initial(bool status);			// nastavenie init pinu 
	  bool initial_status();
	  void pin_select(uint8_t input);		// cislo vybraneho pinu
	  void pin_write(bool status);			// zapis na pin
	  bool pin_read();				// citanie z pinu
};

#endif
