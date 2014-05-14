#include "sharp.h"

void sharp_init()
{
	analogReadResolution(10);
}

float sharp_distance(uint8_t addres)
{
	//float voltage = (analogRead(addres) * (3.3 / 4095));
	float x = (analogRead(addres) * (3.3 / 1024));
	//return voltage;
	//return 10650.08 * pow(analogRead(addres),-0.935) - 10;
	//return (1 / (0.0002391473 * analogRead(addres) - 0.0100251467));
	//return  65*pow(voltage, -1.10);
	//return (1 / (0.0002391473 * voltage - 0.0100251467));
	//return (analogRead(addres) * (3.3 / 4095));
	//
	//return (306.439 + voltage * (-512.611 + voltage * (382.268 + voltage * (-129.893 + voltage * 16.2537))));
	//return (6050/voltage);
	//return (16.2537 * (x*x*x*x) - 129.893 * (x*x*x) + 382.268 * (x*x) - 512.611 * x + 306.439);
	return (306.439 + x * ( -512.611 + x * ( 382.268 + x * (-129.893 + x * 16.2537) )));
	
}
