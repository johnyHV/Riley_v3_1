#include "mag_and_acc.h"

void initial_mag()
{
	initial_device(0x02, 0x00,Compas);
}

void initial_acc()
{
	initial_device(0x31, 0x01,Acc);
	initial_device(0x2D, 0x08,Acc);
}

void initial_device(byte address, byte val,int DEVICE) {
	Wire.beginTransmission(DEVICE); 	// start transmission to device 
	Wire.write(address);             	// send register address
	Wire.write(val);                 	// send value to write
	Wire.endTransmission();         	// end transmission
}

void read_data(byte address, acc_mag *data,int DEVICE) {
	Wire.beginTransmission(DEVICE);
	Wire.write(byte(address));       	// poslanie poziadavky pre X MSB register
	Wire.endTransmission();
	Wire.requestFrom(DEVICE, 6);    	// ziadanie 6 bytes; 2 bytes pre os
	
	if(Wire.available() <=6) {      	// pokial je 6 bytes k dispozicii
		data->x = Wire.read() << 8 | Wire.read();
		data->y = Wire.read() << 8 | Wire.read();
		data->z = Wire.read() << 8 | Wire.read();
	}
}

uint8_t kalibracia(uint8_t &v_c_x_constant,uint8_t &v_c_y_constant,uint8_t &v_c_z_constant) {
/*	int c_x_max,c_y_max,c_z_max,a_x_max,a_y_max,a_z_max=0;
	int c_x_min,c_y_min,c_z_min,a_x_min,a_y_min,a_z_min=255;
	
	//for (int i=0;i<8500;i++)
	for (int i=0;i<17000;i++)
	{
		//motor_step(&motor_l,&motor_r,true,true);
		//delayMicroseconds(1400);
		//Serial.println(i);
		//extremy kompasu
		Wire.beginTransmission(Compas);
		Wire.write(byte(0x03));       // poslanie poziadavky pre X MSB register
		Wire.endTransmission();
		Wire.requestFrom(Compas, 6);    // ziadanie 6 bytes; 2 bytes pre os
		if(Wire.available() <=6) {      // pokial je 6 bytes k dispozicii
			uint8_t x = Wire.read() << 8 | Wire.read();
			uint8_t y = Wire.read() << 8 | Wire.read();
			uint8_t z = Wire.read() << 8 | Wire.read();
			
			Serial.print("x: ");
			Serial.print(x);
			Serial.print(",y: ");
			Serial.print(y);
			Serial.print(",z: ");
			Serial.println(z);
			if (x > c_x_max)
				c_x_max = x;
			else if (x < c_x_min)
				c_x_min = x;
				
			if (y > c_y_max)
				c_y_max = y;
			else if (y < c_y_min)
				c_y_min = y;
				
			if (z > c_z_max)
				c_z_max = z;
			else if (z < c_z_min)
				c_z_min = z;
		}
	}
	
	Serial.println(c_x_max);
	Serial.println(c_x_min);
	v_c_x_constant = c_x_max - c_x_min;
	v_c_y_constant = c_y_max - c_y_min;
	v_c_z_constant = c_z_max - c_z_min;*/
}

int azimuth_cisty() {
	acc_mag mag = {0,0,0};
	
	//Wire.write(byte(0x03)); 
	read_data( 0x03, &mag ,Compas);
	
	return int(RadiansToDegrees(atan2(mag.x,mag.y)));
}

int azimuth_acc() {
	float x_mag,z_mag,y_mag,x_acc,y_acc,z_acc=0;
	acc_mag mag = {0,0,0};
	acc_mag acc = {0,0,0};
	
	//vycitanie kompasa
	read_data( 0x03, &mag,Compas); 
	
	//vycitanie acc
	//read_data(DATAX0, &acc,Acc);
	read_data(0x32, &acc,Acc);
	
	float rollRadians = atan2(acc.y,acc.z);
	float pitchRadians = atan2(acc.x,acc.z);
 
	// We cannot correct for tilt over 40 degrees with this algorthem, if the board is tilted as such, return 0.
	if(rollRadians > 0.78 || rollRadians < -0.78 || pitchRadians > 0.78 || pitchRadians < -0.78)
	{
		return 0;
	}
  
	// Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
	float cosRoll = cos(rollRadians);
	float sinRoll = sin(rollRadians);  
	float cosPitch = cos(pitchRadians);
	float sinPitch = sin(pitchRadians);
  
	// The tilt compensation algorithem.
	float Xh = mag.x * cosPitch + mag.z * sinPitch;
	float Yh = mag.x * sinRoll * sinPitch + mag.y * cosRoll - mag.z * sinRoll * cosPitch;
  
	float heading = atan2(Yh, Xh);
	return RadiansToDegrees(heading);
	
	
	/*float rollRadians = atan2(acc[1],acc[2]);
	float pitchRadians = atan2(acc[0],acc[2]);
 
	// We cannot correct for tilt over 40 degrees with this algorthem, if the board is tilted as such, return 0.
	if(rollRadians > 0.78 || rollRadians < -0.78 || pitchRadians > 0.78 || pitchRadians < -0.78)
	{
		return 0;
	}
  
	// Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
	float cosRoll = cos(rollRadians);
	float sinRoll = sin(rollRadians);  
	float cosPitch = cos(pitchRadians);
	float sinPitch = sin(pitchRadians);
  
	// The tilt compensation algorithem.
	float Xh = mag[0] * cosPitch + mag[2] * sinPitch;
	float Yh = mag[0] * sinRoll * sinPitch + mag[1] * cosRoll - mag[2] * sinRoll * cosPitch;
  
	float heading = atan2(Yh, Xh);
	return RadiansToDegrees(heading);*/
	
}

float RadiansToDegrees(float rads)
{
  if(rads < 0)
    rads += 2*PI;
      
  if(rads > 2*PI)
    rads -= 2*PI;
   
  float heading = rads * 180/PI;
       
  return heading;
}

void read_mag(acc_mag *data) 
{
	acc_mag mag = {0,0,0};

	read_data( 0x03, &mag ,Compas);
	
	data->x = mag.x;
	data->y = mag.y;
	data->z = mag.z;
}

void read_acc(acc_mag *data)
{
	acc_mag acc = {0,0,0};
	
	read_data(0x32, &acc,Acc);
	
	data->x = acc.x;
	data->y = acc.y;
	data->z = acc.z;
}
