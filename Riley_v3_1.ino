/*------------------------------------------------------------------------------
 *              Projekt: Riley 3.1
 *              Autor: Miroslav Pivovarsky
 *              Kontakt: miroslav.pivovarsky@iklub.sk
 * -----------------------------------------------------------------------------
 */

/*
 w - dopredu
a - vlavo
d- v pravo
s - vzad
e -  stop
q - ziskanie dat z kompasu
c - ziskanie statusu kritickych bodov
z - vybratie IR senzora
t - ziskanie teplot z LM75
u - ziskanie dat z ultrazvuku
i - ziskanie dat z sharpu
y - scan priestoru
k - kontrola vzdialenosti
h - snimacia hlava predna
l - testovaci algoritmus
x - detekcia schodov
p - prezentacia stol
n - nastavenie povrchu pre jazdenie: p - podlaha,k-koberec
o - prezentacia stol, bez prednych proximitnych
 *  
 */

#include <Arduino.h>

#include <Wire.h>
#include <Scheduler.h>
//#include "mux4067.h"
#include "premenne.h"
#include "drv8805_.h"
#include "proximity.h"
#include "ultrasonic.h"
#include "LM75.h"
#include "mag_and_acc.h"
#include "calculation.h"
#include "system.h"
#include <SPI.h>
#include <SD.h>


File myFile;
bool tmp_1 = true;

//teplotne senzory
LM75 temp_u(0x48); // 72 - horny teplotny senzor
LM75 temp_d(0x49); // 73 - dolny teplotny senzor

//kalibracne konstatny
uint8_t c_x_constant, c_y_constant, c_z_constant = 0;
uint8_t a_constant[3] = {0};

int stepst = 0;

void scan();
void hladaj_sever();
//void motor_loop();

//------------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Setup >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//------------------------------------------------------------------------------

void setup() {
    pinMode(4, OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(7, OUTPUT);
    digitalWrite(4, HIGH);
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
    digitalWrite(7, LOW);
    Serial.begin(9600);
    Serial.print("AT+NAMERiley");
    Serial.println("+-------------------------+");
    Serial.println("| Start Boot");

    Serial.println("|");
    Serial.println("| Multiplexor");

    Serial.println("|");
    Serial.println("| Motor");
    motor_initial(&motor_l); //inicializacia laveho motora
    motor_initial(&motor_r); //inicializacia praveho motora
    motor_initial(&motor_f); //inicializacia predneho motora
    motor_initial(&motor_b); //inicializacina zadneho motora
    motor_enable(&motor_l, &motor_r, false, false); //vypnutie pohonovych motorov
    motor_enable(&motor_f, &motor_b, false, false); //vypnutie motorov pre hlavy
    motor_set_mod(&motor_l, 0);
    motor_f.smer_otocenia = CENTER;
    motor_b.smer_otocenia = CENTER;

    Serial.println("|");
    Serial.println("| I2C");
    Wire.begin(); 

    Serial.println("|");
    Serial.println("| Compas");
    acc_mag pamet = {0, 0, 0}; // X,Y,Z os akcelerometer
    initial_mag();
    read_mag(&pamet);

    Serial.print("| X:");
    Serial.print(pamet.x);
    Serial.print(" y:");
    Serial.print(pamet.y);
    Serial.print(" z:");
    Serial.println(pamet.z);
    Serial.print("| Azimuth mag: ");
    Serial.println(azimuth_cisty());

    Serial.println("|");
    Serial.println("| Akcelerometer");
    initial_acc();
    read_acc(&pamet);
    Serial.print("| X:");
    Serial.print(pamet.x);
    Serial.print(" y:");
    Serial.print(pamet.y);
    Serial.print(" z:");
    Serial.println(pamet.z);
    Serial.print("| Azimuth mag + acc: ");
    Serial.println(azimuth_acc());

    Serial.println("|");
    Serial.println("| Proximity");
    proximity_initial(&proximity, mux); //inicializacia proximity
    Serial.print("| Critical: ");
    Serial.println(proximity_critical_status(&proximity));
    Serial.print("| Schody: ");
    Serial.println(schody());
    sharp_init();
    Serial.print("| Fron sharp: ");
    Serial.println(sharp_distance(sharp_f));
    Serial.print("| Fron back: ");
    Serial.println(sharp_distance(sharp_b));

    Serial.println("|");
    Serial.println("| Ultrasonic");
    ultrasonic_initial(&uz_f);
    ultrasonic_initial(&uz_b);
    Serial.print("| Fron ultra sonic: ");
    Serial.println(ultrasonic_distance(&uz_f));
    Serial.print("| Back ultra sonic: ");
    Serial.println(ultrasonic_distance(&uz_b));

    Serial.println("|");
    Serial.println("| Tempeature");
    //Serial.print("| Tempeature CPU: ");
    //Serial.println(GetTemp());
    Serial.print("| Tempeature UP: ");
    Serial.print(temp_u.temp());
    Serial.println(" C");
    Serial.print("| Tempeature DOWN: ");
    Serial.print(temp_d.temp());
    Serial.println(" C");
    temp_u.shutdown(true);
    temp_d.shutdown(true);

    Serial.println("|");
    Serial.println("| start loop");
    Scheduler.startLoop(motor_loop);
    //Scheduler.startLoop(loop3);

    Serial.println("|");
    Serial.println("| Start");
    Serial.println("+-------------------------+");

    //Serial.println(ultrasonic_distance_(&uz_f,1));
    digitalWrite(4, LOW);
    digitalWrite(7, HIGH);

    /*
      if (!SD.begin(44)) {
            Serial.println("initialization failed!");
            return;
      }
    Serial.println("initialization done.");
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
Serial.print("Writing to test.txt...");
myFile.print("testing 1, 2, 3.");
// close the file:
myFile.close();
Serial.println("done.");
    } else {
// if the file didn't open, print an error:
Serial.println("error opening test.txt");
    }
    myFile = SD.open("test.txt");
    if (myFile) {
Serial.println("Read test.txt:");
    
Serial.println(myFile.position());
Serial.println(myFile.peek());
Serial.println(myFile.seek(2));
Serial.println(myFile.peek());
    
// read from the file until there's nothing else in it:
while (myFile.available()) {
  Serial.write(myFile.read());
}
// close the file:
myFile.close();
    } else {
// if the file didn't open, print an error:
Serial.println("error opening test.txt");
    }
     */
}

void loop() {
    if (Serial.available() > 0) {
        int incomingByte = Serial.read();

        if (incomingByte == 'w') { // w - dopredu
            motor_l.l_enable = true;
            motor_l.l_dir = true;
            motor_r.l_enable = true;
            motor_r.l_dir = false;
            b_mod[0] = true;
            b_mod[1] = false;
            b_mod[2] = true;
            b_mod[3] = true;
            motor_enable(&motor_l, &motor_r, true, true);
        } else if (incomingByte == 'a') { // a - vlavo
            motor_enable(&motor_l, &motor_r, true, true);
            motor_l.l_enable = true;
            motor_l.l_dir = false;
            motor_r.l_enable = true;
            motor_r.l_dir = false;
            b_mod[0] = false;
            b_mod[1] = false;
            b_mod[2] = true;
            b_mod[3] = true;
        } else if (incomingByte == 'd') { // d- v pravo
            motor_enable(&motor_l, &motor_r, true, true);
            motor_l.l_enable = true;
            motor_l.l_dir = true;
            motor_r.l_enable = true;
            motor_r.l_dir = true;
            b_mod[0] = true;
            b_mod[1] = true;
            b_mod[2] = true;
            b_mod[3] = true;
        } else if (incomingByte == 's') { // s - vzad
            motor_enable(&motor_l, &motor_r, true, true);
            motor_l.l_enable = true;
            motor_l.l_dir = false;
            motor_r.l_enable = true;
            motor_r.l_dir = true;
            b_mod[0] = false;
            b_mod[1] = true;
            b_mod[2] = true;
            b_mod[3] = true;
        } else if (incomingByte == 'e') { // e -  stop
            motor_enable(&motor_l, &motor_r, false, false);
            motor_l.l_enable = false;
            motor_r.l_enable = false;
            b_mod[0] = false;
            b_mod[1] = false;
            b_mod[2] = false;
            b_mod[3] = false;
            Serial.print("Prejdena vzdialenost: ");
            Serial.println(motor_distance(&motor_l));
        } else if (incomingByte == 'q') { // q - ziskanie dat z kompasu
            Serial.print("Azimuth cisty: ");
            Serial.println(azimuth_cisty());
            Serial.print("Azimuth cisty scaled : ");
            Serial.println(azimuth_cisty_scaled());
            Serial.print("Azimuth acc: ");
            Serial.println(azimuth_acc());
        } else if (incomingByte == 'c') { // c - ziskanie statusu kritickych bodov
            Serial.print("Critical: ");
            Serial.println(proximity_critical_status(&proximity));
        } else if ((incomingByte >= 48) && (incomingByte <= 57)) { // z - vybratie IR senzora
            Serial.println(incomingByte - 48);
            Serial.print("Vystup: ");
            Serial.println(proximity_d_status(&proximity, incomingByte - 48, mux));
            Serial.println(proximity_a_status(&proximity, incomingByte - 48, mux));
        } else if (incomingByte == 't') { // t - ziskanie teplot z LM75
            temp_u.shutdown(false);
            temp_d.shutdown(false);
            Serial.print("Tempeature UP: ");
            Serial.print(temp_u.temp());
            Serial.println(" C");
            Serial.print("Tempeature DOWN: ");
            Serial.print(temp_d.temp());
            Serial.println(" C");
            temp_u.shutdown(true);
            temp_d.shutdown(true);
        } else if (incomingByte == 'u') { // u - ziskanie dat z ultrazvuku
            Serial.print("Predny ultrazvuk f: ");
            Serial.println(ultrasonic_distance(&uz_f));
            Serial.print("Po korekcii f: ");
            Serial.println(ultrasonic_distance_(&uz_f));
            Serial.print("Po korekcii b: ");
            Serial.println(ultrasonic_distance_(&uz_b));
        } else if (incomingByte == 'i') { // i - ziskanie dat z sharpu
            Serial.print("Predny sharp: ");
            Serial.println(sharp_distance(sharp_f));
            Serial.print("Zadny sharp: ");
            Serial.println(sharp_distance(sharp_b));
        } else if (incomingByte == 'y') { // y - scan priestoru
            scan();
        } else if (incomingByte == 'k') { // k - kontrola vzdialenosti
            Serial.println(senzor_check(FRONT));
        } else if (incomingByte == 'h') { // h - snimacia hlava predna
            Serial.print("Front: ");
            Serial.println(head_distance(uz_f, sharp_f));
            Serial.print("Back: ");
            Serial.println(head_distance(uz_b, sharp_b));
        } else if (incomingByte == 'x') { // x - detekcia schodov
            Serial.print("Schody: ");
            Serial.println(schody());
            Serial.print("Zadny: ");
            Serial.println(schody_back());

        } else if (incomingByte == 'n') { // n - nastavenie povrchu pre jazdenie
            int incomingByte = Serial.read();
            if (incomingByte == 'k')
                wheel.d = 26;
            if (incomingByte == 'p')
                wheel.d = 26.52;
            Serial.print("Konstanta pre otocenie: ");
            Serial.println(wheel.d);
        } else if (incomingByte == 'l') { // l - testovaci algoritmus
            uint8_t i = 0;

            head_reset(&motor_f);
            rotate_head_angle(FRONT, 90, RIGHT);
            while (motor_f.l_enable)
                delay(11);
            uint8_t right = head_distance(uz_f, sharp_f);
            head_center(FRONT);
            while (motor_f.l_enable)
                delay(11);
            head_reset(&motor_f);
            rotate_head_angle(FRONT, 90, LEFT);
            while (motor_f.l_enable)
                delay(11);
            uint8_t left = head_distance(uz_f, sharp_f);
            head_center(FRONT);
            while (motor_f.l_enable)
                delay(11);
            bool smer = false;
            if (left > right)
                smer = RIGHT;
            
            while (i < 2) {
                i++;
                vzdialenost_stena(smer);

                head_reset(&motor_l); // zapnem motory
                head_reset(&motor_r);
                motor_l.l_dir = true;
                motor_r.l_dir = false;
                motor_enable(&motor_l, &motor_r, true, true);
                motor_l.l_enable = true;
                motor_r.l_enable = true;

                while (!proximity_critical_status(&proximity)) {
                   // if (schody()) {
                    //    motor_enable(&motor_l, &motor_r, false, false);
                    //    motor_l.l_enable = false;
                    //    motor_r.l_enable = false;
                    //    while(true)
                    //        ;
                   // }
                    delay(11);
                }

                motor_enable(&motor_l, &motor_r, false, false);
                motor_l.l_enable = false;
                motor_r.l_enable = false;

                //            prekazka(smer);
                test(smer);
            }
            vzdialenost_stena(smer);
        } else if(incomingByte == 'p') {
            prezentacia_stol();
        } else if(incomingByte == 'o') {
            prezentacia_stol_e();
        }
        Serial.print("I received: ");
        Serial.println(incomingByte);
    }
    delay(250);
}

void motor_loop() {


    // motory pre pohyb
    if ((motor_r.l_enable)&&(motor_l.l_enable)) {
        motor_step(&motor_l, &motor_r, &wheel, motor_l.l_dir, motor_r.l_dir);
        if (motor_l.l_kroky != 0) {
            if ((motor_l.l_kroky == motor_l.number_step)&&(motor_l.full_step == motor_l.l_full_step)) {
                motor_l.l_enable = false;
                motor_r.l_enable = false;
                motor_enable(&motor_l, &motor_r, false, false);
            }
        }
    } else if (motor_l.l_enable)
        motor_step(&motor_l, &wheel, motor_l.l_dir);
    else if (motor_r.l_enable)
        motor_step(&motor_r, &wheel, motor_r.l_dir);

    // motory pre otacanie hlav
    if (motor_f.l_enable) {
        if ((!motor_l.l_enable) || (!motor_r.l_enable))
            motor_step(&motor_f, &wheel, motor_f.l_dir, true);
        else
            motor_step(&motor_f, &wheel, motor_f.l_dir, false);

        if (motor_f.l_kroky != 0) {
            if (motor_f.number_step == motor_f.l_kroky) {
                motor_f.l_enable = false;
                motor_enable(&motor_f, false);
            }
        }
    }
    if (motor_b.l_enable) {
        if ((!motor_l.l_enable) || (!motor_r.l_enable))
            motor_step(&motor_b, &wheel, motor_b.l_dir, true);
        else
            motor_step(&motor_b, &wheel, motor_b.l_dir, false);
        if (motor_b.l_kroky != 0) {
            if (motor_b.number_step == motor_b.l_kroky) {
                motor_b.l_enable = false;
                motor_enable(&motor_b, false);

            }
        }
    }

    delayMicroseconds(1850);
    yield();
}

void scan() {
    motor_set_mod(&motor_l, 0);
    motor_enable(&motor_b, &motor_f, true, true);
    int kroky = 0;

    //lava strana
    for (int c = 0; c < 500; c++) {
        motor_step(&motor_b, &motor_f, &wheel, true, true);
        kroky++;
        delay(2);
        if ((kroky == 100) || (kroky == 200) || (kroky == 300) || (kroky == 400) || (kroky == 500)) {
            Serial.print("Ultrasonic: ");
            Serial.println(ultrasonic_distance_(&uz_f));
            Serial.print("Sharp: ");
            Serial.println(sharp_distance(sharp_f));
        }
    }

    for (; kroky != 0; kroky--) {
        motor_step(&motor_b, &motor_f, &wheel, false, false);
        delay(2);
    }


    // prava strana
    for (int c = 0; c < 500; c++) {
        motor_step(&motor_b, &motor_f, &wheel, false, false);
        kroky++;
        delay(2);
        if ((kroky == 100) || (kroky == 200) || (kroky == 300) || (kroky == 400) || (kroky == 500)) {
            Serial.print("Ultrasonic: ");
            Serial.println(ultrasonic_distance_(&uz_f));
            Serial.print("Sharp: ");
            Serial.println(sharp_distance(sharp_f));
        }
    }



    for (; kroky != 0; kroky--) {
        motor_step(&motor_b, &motor_f, &wheel, true, true);
        delay(2);
    }
    motor_enable(&motor_b, &motor_f, false, false);
}

void hladaj_sever() {
    uint8_t sampleTime = 10;
    if (kontrola) {
        // Compass
        int x, y, z;
        unsigned long time, times;
        int x_new, y_new, z_new, x_old, y_old, z_old;
        int16_t time_c = 0;
        time = millis();

        while (time_c != 1) {
            times = millis() - time;
            if (times > sampleTime) {
                // Inicializacia komunikacie s kompasom
                Wire.beginTransmission(Compas);
                Wire.write(byte(0x03)); // poslanie poziadavky pre X MSB register
                Wire.endTransmission();
                time = millis();
                time_c++;
                Wire.requestFrom(Compas, 6); // ziadanie 6 bytes; 2 bytes pre os
                if (Wire.available() <= 6) { // pokial je 6 bytes k dispozicii
                    x_new = Wire.read() << 8 | Wire.read();
                    y_new = Wire.read() << 8 | Wire.read();
                    z_new = Wire.read() << 8 | Wire.read();
                }

                if (time_c = 1) {
                    x_old = x_new;
                    y_old = y_new;
                    z_old = z_new;
                }
            }
        }

        x = (x_old + x_new) / 2 * times;
        y = (y_old + y_new) / 2 * times;
        z = (z_old + z_new) / 2 * times;


        int azimut = int(atan((double) y / (double) x)*(360 / PI));
        Serial.println(azimut);
        
        if (azimut > 0)
            motor_step(&motor_l, &motor_r, &wheel, true, true);
        else if (azimut < 0)
            motor_step(&motor_l, &motor_r, &wheel, false, false);
        else
            kontrola = false;
    }
}

