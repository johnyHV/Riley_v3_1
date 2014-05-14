#ifndef _mag_and_acc_h_
#define _mag_and_acc_h_


// azimuth +90 stupnov == sever

#include <Arduino.h>
#include <Wire.h>
#include "premenne.h"

//compass
#define Compas 0x1E

#define HMC5883L_Address 0x1E
#define ConfigurationRegisterA 0x00
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define DataRegisterBegin 0x03

#define Measurement_Continuous 0x00
#define Measurement_SingleShot 0x01
#define Measurement_Idle 0x03

//akcelerometer
#define Acc 0x53
//char POWER_CTL = 0x2D;       //Power Control Register
//char DATA_FORMAT = 0x31;
//char DATAX0 = 0x32;        	 //X-Axis Data 0
//char DATAX1 = 0x33;        //X-Axis Data 1
//char DATAY0 = 0x34;        //Y-Axis Data 0
//char DATAY1 = 0x35;        //Y-Axis Data 1
//char DATAZ0 = 0x36;        //Z-Axis Data 0
//char DATAZ1 = 0x37;        //Z-Axis Data 1

void initial_device(byte address, byte val, int DEVICE);
void read_data(byte address, acc_mag *data, int DEVICE);
void initial_mag();
float SetScale(float gauss);
void SetMeasurementMode(uint8_t mode);
void ReadScaledAxis(acc_mag *data);
void initial_acc();
void read_mag(acc_mag *data);
void read_acc(acc_mag *data);
uint8_t kalibracia(uint8_t &v_c_x_constant,uint8_t &v_c_y_constant,uint8_t &v_c_z_constant);
int azimuth_cisty();
int azimuth_cisty_scaled();
int azimuth_acc();
float RadiansToDegrees(float rads);


#endif
