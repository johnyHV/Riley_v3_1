#ifndef _mag_and_acc_h
#define _mag_and_acc_h

#include <Arduino.h>
#include <Wire.h>
#include "premenne.h"

//compass
#define Compas 0x1E

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

void initial_mag();
void initial_acc();
void initial_device(byte address, byte val, int DEVICE);
void read_data(byte address, acc_mag *data, int DEVICE);
uint8_t kalibracia(uint8_t &v_c_x_constant,uint8_t &v_c_y_constant,uint8_t &v_c_z_constant);
int azimuth_cisty();
int azimuth_acc();
float RadiansToDegrees(float rads);
void read_mag(acc_mag *data);
void read_acc(acc_mag *data);

#endif
