#include "mag_and_acc.h"

/**
 *
 * @info Inicializaciu acc a mag senzora
 * @param address - adresa zariadenia
 * @param val - poslanie hodnoty pre zapis
 * @param device - 
 * @return void
 */
void initial_device(byte address, byte val,int DEVICE) {
	Wire.beginTransmission(DEVICE); 	// start transmission to device 
	Wire.write(address);             	// send register address
	Wire.write(val);                 	// send value to write
	Wire.endTransmission();         	// end transmission
}

/**
 *
 * @info Citanie dat z ACC a MAG senzora
 * @param address - adresa zariadenia
 * @param data - struktura x,y,z pre ulozenie vycitanych dat
 * @return void
 */
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

/**
 *
 * @info Inicializacia Magnetickeho senzora HCM5883
 * @return void
 */
void initial_mag()
{
	initial_device(0x02, 0x00,Compas);
        
        SetScale(1.3);
        SetMeasurementMode(Measurement_Continuous);
}


float SetScale(float gauss)
{
    float Scale=0;
	uint8_t regValue = 0x00;
	if(gauss == 0.88)
	{
		regValue = 0x00;
		Scale = 0.73;
	}
	else if(gauss == (float) 1.30)
	{
		regValue = 0x01;
		Scale = 0.92;
	}
	else if(gauss == 1.9)
	{
		regValue = 0x02;
		Scale = 1.22;
	}
	else if(gauss == 2.5)
	{
		regValue = 0x03;
		Scale = 1.52;
	}
	else if(gauss == 4.0)
	{
		regValue = 0x04;
		Scale = 2.27;
	}
	else if(gauss == 4.7)
	{
		regValue = 0x05;
		Scale = 2.56;
	}
	else if(gauss == 5.6)
	{
		regValue = 0x06;
		Scale = 3.03;
	}
	else if(gauss == 8.1)
	{
		regValue = 0x07;
		Scale = 4.35;
	}
	else
		
	
	// Setting is in the top 3 bits of the register.
	regValue = regValue << 5;
	initial_device(ConfigurationRegisterB, regValue, Compas);
        return Scale;
}

void SetMeasurementMode(uint8_t mode)
{
    initial_device(ModeRegister, mode, Compas);
}

void ReadScaledAxis(acc_mag *data){
    acc_mag raw;
    read_mag(&raw);
    float Scale = SetScale(1.3);
    data->x = raw.x * Scale;
    data->y = raw.z * Scale;
    data->z = raw.z * Scale;
}

/**
 *
 * @info Inicializacia Akcelerometra senzora ADXL345
 * @return void
 */
void initial_acc()
{
	initial_device(0x31, 0x01,Acc);
	initial_device(0x2D, 0x08,Acc);
}

/**
 *
 * @info Vycitanie hodnot z kompasu
 * @param data - x,y,z struktura acc_mag
 * @return void
 */
void read_mag(acc_mag *data) 
{
	acc_mag mag = {0,0,0};

	read_data( 0x03, &mag ,Compas);
	
	data->x = mag.x;
	data->y = mag.y;
	data->z = mag.z;
}

/**
 *
 * @info Vycitanie hodnot z akcelerometra
 * @param data - x,y,z struktura acc_mag
 * @return void
 */
void read_acc(acc_mag *data)
{
	acc_mag acc = {0,0,0};
	
	read_data(0x32, &acc,Acc);
	
	data->x = acc.x;
	data->y = acc.y;
	data->z = acc.z;
}

/**
 *
 * @info kalibracia akcelerometra a kompasu
 * @return void
 */
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


/**
 *
 * @info Vypocet azimuthu z osy X a Y kompasa
 * @return azimuth v stupnoch
 */
int azimuth_cisty() {
	acc_mag mag = {0,0,0};
	
	//Wire.write(byte(0x03)); 
	read_data( 0x03, &mag ,Compas);
	
	return int(RadiansToDegrees(atan2(mag.x,mag.y)));
}

/**
 *
 * @info Vypocet azimuthu z osy X a Y kompasa po korekcii teploty
 * @return azimuth v stupnoch
 */
int azimuth_cisty_scaled() {
    float Scale = SetScale(1.3);
    acc_mag data = {0,0,0};
    acc_mag mag = {0,0,0};
    ReadScaledAxis(&data);
    mag.x = data.x * Scale;
    mag.y = data.y * Scale;
    mag.z = data.z * Scale;
    
    return int(RadiansToDegrees(atan2(mag.x,mag.y)));
}

/**
 *
 * @info Vypocet azimuthu + korekcia naklonenia pomocou akcelerometra
 * @return azimuth v stupnoch
 */
int azimuth_acc() {
	float x_mag,z_mag,y_mag,x_acc,y_acc,z_acc=0;
	acc_mag mag = {0,0,0};
	acc_mag acc = {0,0,0};
	
	//vycitanie kompasa
        float Scale = SetScale(1.3);
        acc_mag data = {0,0,0};
        ReadScaledAxis(&data);
        mag.x = data.x * Scale;
        mag.y = data.y * Scale;
        mag.z = data.z * Scale;
	//read_data( 0x03, &mag,Compas); 
	
	//vycitanie acc
	//read_data(DATAX0, &acc,Acc);
	read_data(0x32, &acc,Acc);
        acc.x = -acc.x;
	
	float rollRadians = atan2(acc.y,acc.z);
	float pitchRadians = atan2(acc.x,acc.z);
        //float rollRadians = asin(acc.y);
        //float pitchRadians = asin(acc.x);
 
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
	float Xh = ((float) mag.x) * cosPitch + ((float) mag.z) * sinPitch;
	float Yh = ((float) mag.x) * sinRoll * sinPitch + ((float) mag.y) * cosRoll - ((float) mag.z) * sinRoll * cosPitch;
  /*
        Serial.println(acc.x);
        Serial.println(acc.y);
        Serial.println(acc.z);
        
        Serial.println(mag.x);
        Serial.println(mag.y);
        Serial.println(mag.z);
        
        Serial.println(rollRadians);
        Serial.println(pitchRadians);
        
        Serial.println(cosRoll);
        Serial.println(sinRoll);
        Serial.println(cosPitch);
        Serial.println(sinPitch);
        
        Serial.println(Xh);
        Serial.println(Yh);
        */
        
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

/**
 *
 * @info Funkcia na prevod radianov na stupne
 * @return stupne
 */
float RadiansToDegrees(float rads)
{
  if(rads < 0)
    rads += 2*PI;
      
  if(rads > 2*PI)
    rads -= 2*PI;
   
  float heading = rads * 180/PI;
       
  return heading;
}

