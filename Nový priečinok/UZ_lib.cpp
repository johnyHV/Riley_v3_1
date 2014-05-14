#include "UZ_lib.h"

UZ_lb::UZ_lb(uint8_t _echo, uint8_t _trig)
{
	echo_pin = _echo;
	trig_pin = _trig;
	pinMode(trig_pin,OUTPUT);
	pinMode(echo_pin,INPUT);
}

UZ_lb::UZ_lb(uint8_t _echo, uint8_t _trig, uint8_t (&v_mot)[4])
{
	motor = new motor_lb(v_mot);
	
	echo_pin = _echo;
	trig_pin = _trig;
	pinMode(trig_pin,OUTPUT);
	pinMode(echo_pin,INPUT);
	
	r_left = r_right = 0;
}

int UZ_lb::scan(uint8_t rotate, uint8_t step)
{
	if (1 == rotate)
	{
		for (uint8_t g=0;g<step;g++)
		{
			motor->o_mot_left();
			r_left++;
		}
				
		motor->o_mot_stop();
		return (UZ_timing());		
	}
	else if (2 == rotate)
	{
		for (uint8_t g=0;g<step;g++)
		{
			motor->o_mot_right();
			r_right++;
		}

		motor->o_mot_stop();
		return (UZ_timing());
	}
	else
		return (-1);
}

bool UZ_lb::center()
{
	if (r_left > 0)
	{
		for (;r_left;r_left--)
			motor->o_mot_right();
			
		motor->o_mot_stop();			
		return true;
	}
	else if (r_right > 0)
	{
		for (;r_right;r_right--)
			motor->o_mot_left();
			
		motor->o_mot_stop();
		return true;
	}
	else
		return false;
}

int UZ_lb::pos_left()
{
	return r_left;
}
int UZ_lb::pos_right()
{
	return r_right;
}

int UZ_lb::UZ_timing()
{
	int t_micro = micros();
	int temp_b = t_micro + 21000;
	
	digitalWrite(trig_pin, LOW);
	delayMicroseconds(20);
		
	digitalWrite(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig_pin, LOW);
	
	int temp = pulseIn(echo_pin, HIGH, 20000);
	
	if (micros() <= temp_b)
	{
		temp = temp / 58;
		delay(50);		
	}
	else
	{
		temp = 400;
	}
	
	return temp;
}

