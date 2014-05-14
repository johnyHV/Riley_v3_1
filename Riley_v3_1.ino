/*------------------------------------------------------------------------------
 *              Projekt: Riley 3.1
 *              Autor: Miroslav Pivovarsky
 *              Kontakt: miroslav.pivovarsky@iklub.sk
 * -----------------------------------------------------------------------------
 */

#include <Arduino.h>
#include <Wire.h>
//#include "mux4067.h"
#include "premenne.h"
#include "drv8805_.h"
#include "proximity.h"
#include "ultrasonic.h"
#include "LM75.h"
#include "mag_and_acc.h"
#include "calculation.h"
#include <Scheduler.h>

//teplotne senzory
LM75 temp_u(0x48);                                              // 72 - horny teplotny senzor
LM75 temp_d(0x49);                                              // 73 - dolny teplotny senzor

//kalibracne konstatny
uint8_t c_x_constant,c_y_constant,c_z_constant=0;
uint8_t a_constant[3]={0};

int stepst = 0;

void scan();
void hladaj_sever();

//------------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Setup >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//------------------------------------------------------------------------------
void setup() {
    pinMode(4,OUTPUT);
    pinMode(7,OUTPUT);
    digitalWrite(4,HIGH);
    digitalWrite(7,LOW);
	Serial.begin(9600);
	Serial.println("+-------------------------+");
	Serial.println("| Start Boot");

        Serial.println("|");Serial.println("| Multiplexor");
	
        Serial.println("|");Serial.println("| Motor");
	motor_initial(&motor_l);				//inicializacia laveho motora
	motor_initial(&motor_r);				//inicializacia praveho motora
	motor_initial(&motor_f);                            //inicializacia predneho motora
	motor_initial(&motor_b);                            //inicializacina zadneho motora
	motor_enable(&motor_l,&motor_r,false,false);            //vypnutie pohonovych motorov
	motor_enable(&motor_f,&motor_b,false,false);            //vypnutie motorov pre hlavy
	motor_set_mod(&motor_l,0);
        motor_f.smer_otocenia = CENTER;
        motor_b.smer_otocenia = CENTER;

	Serial.println("|");Serial.println("| I2C");
	Wire.begin();                                           // zapnutie I2C
	
	Serial.println("|");Serial.println("| Compas");
	acc_mag pamet = {0,0,0};                                // X,Y,Z os akcelerometer
	initial_mag();
	read_mag(&pamet);
	
	Serial.print("| X:");Serial.print(pamet.x);
	Serial.print(" y:");Serial.print(pamet.y);
	Serial.print(" z:");Serial.println(pamet.z);
	Serial.print("| Azimuth mag: ");Serial.println(azimuth_cisty());	 
        
	Serial.println("|");Serial.println("| Akcelerometer");
	initial_acc();
	read_acc(&pamet);
	Serial.print("| X:");Serial.print(pamet.x);
	Serial.print(" y:");Serial.print(pamet.y);
	Serial.print(" z:");Serial.println(pamet.z);
	Serial.print("| Azimuth mag + acc: ");Serial.println(azimuth_acc());

	Serial.println("|");Serial.println("| Proximity");
	proximity_initial(&proximity,mux);					//inicializacia proximity
	Serial.print("| Critical: ");Serial.println(proximity_critical_status(&proximity));
	sharp_init();
	Serial.print("| Fron sharp: ");Serial.println(sharp_distance(sharp_f));
        Serial.print("| Fron back: ");Serial.println(sharp_distance(sharp_b));
	
	Serial.println("|");Serial.println("| Ultrasonic");
	ultrasonic_initial(&uz_f);
	ultrasonic_initial(&uz_b);
	Serial.print("| Fron ultra sonic: ");
	Serial.println(ultrasonic_distance(&uz_f));
	Serial.print("| Back ultra sonic: ");
	Serial.println(ultrasonic_distance(&uz_b));
	
	Serial.println("|");Serial.println("| Tempeature");
	Serial.print("| Tempeature UP: ");
	Serial.print(temp_u.temp());Serial.println(" C");
	Serial.print("| Tempeature DOWN: ");
	Serial.print(temp_d.temp());Serial.println(" C");
	temp_u.shutdown(true);
	temp_d.shutdown(true);
	
	Serial.println("|");Serial.println("| Start");
	Serial.println("+-------------------------+");
	
	//Serial.println(ultrasonic_distance_(&uz_f,1));
        Serial.println((((360.0 * 5.0) / (2.0 * 3.14 * 3.4)) / (360.0/2048.0)));
        digitalWrite(4,LOW);
        digitalWrite(7,HIGH);
        

}

void loop() {

	//if (!kontrola) {

/*
	if(b_mod[2]){
            if (stepst >= 8400) {
                b_mod[0]=false; b_mod[1]=false; b_mod[2]=false; b_mod[3]=false;
            }
            stepst++;
        }
     */   
    
				//motor_enable(&motor_l,&motor_r,b_mod[2],b_mod[3]);
                                //motor_step(&motor_l,&motor_r,&wheel,b_mod[0],b_mod[1]);
				//motor_step(&motor_l,&motor_r,&wheel,b_mod[0],b_mod[1],5,b_mod[2],b_mod[3]);
				//delayMicroseconds(2000);
				//Serial.println(motor_l.number_step);
	//}

	if (Serial.available() > 0) {
		int incomingByte = Serial.read();
		
		if ( incomingByte == 119) {							// w - dopredu
			b_mod[0]=true; b_mod[1]=false; b_mod[2]=true; b_mod[3]=true; }
		else if ( incomingByte == 97 ) {						// a - vlavo
			b_mod[0]=false; b_mod[1]=false; b_mod[2]=true; b_mod[3]=true; }
		else if ( incomingByte == 100 ) { 						// d- v pravo
			b_mod[0]=true; b_mod[1]=true; b_mod[2]=true; b_mod[3]=true; }
		else if ( incomingByte == 115 ) { 						// s - vzad
			b_mod[0]=false; b_mod[1]=true; b_mod[2]=true; b_mod[3]=true; }
		else if ( incomingByte == 101 ) { 						// e -  stop
			b_mod[0]=false; b_mod[1]=false; b_mod[2]=false; b_mod[3]=false; }
		else if ( incomingByte == 113 ) { 						// q - ziskanie dat z kompasu
			Serial.print("Azimuth cisty: "); Serial.println(azimuth_cisty());
                        Serial.print("Azimuth cisty scaled : "); Serial.println(azimuth_cisty_scaled());
			Serial.print("Azimuth acc: ");   Serial.println(azimuth_acc()); }
		else if ( incomingByte == 99 ) {						// c - ziskanie statusu kritickych bodov
			Serial.print("Critical: ");
			Serial.println(proximity_critical_status(&proximity)); }
		else if (( incomingByte >= 48 ) && ( incomingByte <= 57 )) {                    // z - vybratie IR senzora
			Serial.println(incomingByte-48);
			Serial.print("Vystup: ");
			Serial.println(proximity_d_status(&proximity,incomingByte-48,mux));	
			Serial.println(proximity_a_status(&proximity,incomingByte-48,mux));}
		else if ( incomingByte == 116 ) {						// t - ziskanie teplot z LM75
			temp_u.shutdown(false);temp_d.shutdown(false);
			Serial.print("Tempeature UP: "); 
			Serial.print(temp_u.temp());Serial.println(" C");
			Serial.print("Tempeature DOWN: ");
			Serial.print(temp_d.temp());Serial.println(" C"); 
			temp_u.shutdown(true);temp_d.shutdown(true);}
		else if ( incomingByte == 117 ) {						// u - ziskanie dat z ultrazvuku
			Serial.print("Predny ultrazvuk f: ");
			Serial.println(ultrasonic_distance(&uz_f));
			Serial.print("Po korekcii f: ");
			Serial.println(ultrasonic_distance_(&uz_f));
                        Serial.print("Po korekcii b: ");
			Serial.println(ultrasonic_distance_(&uz_b));}
		else if ( incomingByte == 105) {						// i - ziskanie dat z sharpu
			Serial.print("Predny sharp: ");
			Serial.println(sharp_distance(sharp_f));
                        Serial.print("Zadny sharp: ");
			Serial.println(sharp_distance(sharp_b));}
		else if ( incomingByte == 121) {						// y - scan priestoru
			scan(); }
                else if (incomingByte == 107) {                                                 // k - kontrola vzdialenosti
                    Serial.println(senzor_check(FRONT));
                }
                else if (incomingByte == 104) {                                                 // h - snimacia hlava predna
                    Serial.print("Front: "); Serial.println(head_distance(uz_f,sharp_f));
                    Serial.print("Back: "); Serial.println(head_distance(uz_b,sharp_b));
                    //otocenie_robota(motor_l,motor_r,wheel,180,LEFT);
                    //motor_f.number_step = rotate_head_angle(motor_f,wheel,90,LEFT);
                    //delay(1000);
                    //Serial.print("Kroky motora v slucke: "); Serial.println(motor_f.number_step);
                    //head_center(motor_f, wheel, RIGHT);y
                }
                else if (incomingByte == 108) {
                    vzdialenost_stena(RIGHT);
                    //delay(5000);
                    //vzdialenost_stena(RIGHT);
                    
                    motor_enable(&motor_l,&motor_r,true,true);
                    while (!proximity_critical_status(&proximity)) {
                        motor_step(&motor_l,&motor_r,&wheel,true,false);
                        delayMicroseconds(1800);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                    
                    prekazka();
                    vzdialenost_stena(RIGHT);

                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i = 0;i<=2048*2;i++){
                        motor_step(&motor_l,&motor_r,&wheel,true,false);
                        delayMicroseconds(1800);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                    
                }
		
		Serial.print("I received: ");
		Serial.println(incomingByte);
	}
}

void scan()
{
	motor_set_mod(&motor_l,0);
	motor_enable(&motor_b,&motor_f,true,true);
	int  kroky = 0;
	
	
	//lava strana
	for (int c=0;c<500;c++)
	{	
		motor_step(&motor_b,&motor_f,&wheel, true,true);	
		kroky++;
		delay(2);
		if ((kroky == 100) || (kroky == 200) || (kroky == 300) || (kroky == 400) || (kroky == 500)){
			Serial.print("Ultrasonic: ");Serial.println(ultrasonic_distance_(&uz_f));
			Serial.print("Sharp: ");Serial.println(sharp_distance(sharp_f));
		}
	}
	
	
	
	for (;kroky!=0;kroky--)
	{
		motor_step(&motor_b,&motor_f,&wheel,false,false);
		delay(2);
	}	
	
	
	// prava strana
	for (int c=0;c<500;c++)
	{	
		motor_step(&motor_b,&motor_f,&wheel,false,false);	
		kroky++;
		delay(2);
		if ((kroky == 100) || (kroky == 200) || (kroky == 300) || (kroky == 400) || (kroky == 500)){
			Serial.print("Ultrasonic: ");Serial.println(ultrasonic_distance_(&uz_f));
			Serial.print("Sharp: ");Serial.println(sharp_distance(sharp_f));
		}
	}
	
	
	
	for (;kroky!=0;kroky--)
	{
		motor_step(&motor_b,&motor_f,&wheel,true,true);
		delay(2);
	}
	motor_enable(&motor_b,&motor_f,false,false);	
}

void hladaj_sever() {
    uint8_t sampleTime = 10;
	if (kontrola)
	{
			// Compass
		int x, y, z;
		unsigned long time,times;
		int x_new,y_new,z_new,x_old,y_old,z_old;
		int16_t time_c=0;
		time=millis();
	
		while(time_c != 1)
		{
			times = millis() - time;
			if(times > sampleTime)
			{	
				// Inicializacia komunikacie s kompasom
				Wire.beginTransmission(Compas);
				Wire.write(byte(0x03));       // poslanie poziadavky pre X MSB register
				Wire.endTransmission();
				time=millis();	
				time_c++;	
				Wire.requestFrom(Compas, 6);    // ziadanie 6 bytes; 2 bytes pre os
				if(Wire.available() <=6) {      // pokial je 6 bytes k dispozicii
					x_new = Wire.read() << 8 | Wire.read();
					y_new = Wire.read() << 8 | Wire.read();
					z_new = Wire.read() << 8 | Wire.read();
				}
	
				if(time_c = 1)
				{
					x_old=x_new;
					y_old=y_new;
					z_old=z_new;
				}
			}
		}
		
		x= (x_old + x_new)/2*times;
		y= (y_old + y_new)/2*times;
		z= (z_old + z_new)/2*times;

		
		int azimut = int(atan((double)y/(double)x)*(360/PI));
		Serial.println(azimut);
    
		if (azimut > 0)
			motor_step(&motor_l,&motor_r,&wheel,true,true);
		else if (azimut < 0)
			motor_step(&motor_l,&motor_r,&wheel,false,false);
			else
				kontrola = false;
	}
 }

