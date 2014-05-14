#ifndef _UZ_lib_h                                                   
#define _UZ_lib_h

#include <Arduino.h>
#include <motor_lib.h>



class UZ_lb {
	private:
	uint8_t r_left,r_right;
	uint8_t trig_pin,echo_pin;
  	uint8_t mot[4];
	motor_lb *motor;
	
	
	public:
	UZ_lb(uint8_t _echo, uint8_t _trig);
	UZ_lb(uint8_t _echo, uint8_t _trig, uint8_t (&mot)[4]);
	
	int scan(uint8_t rotate, uint8_t step=10);		// rotate == 1 - vlavo, rotate == 2 - vpravo
	bool center();
	int pos_left();
	int pos_right();
	int UZ_timing();
	
	~UZ_lb() {};
};

#endif
