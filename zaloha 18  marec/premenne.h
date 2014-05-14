#ifndef _premenne_h                                                   
#define _premenne_h

#include <Arduino.h>
//#include "mux4067.h"
//#include "LM75.h"
#define FRONT 1
#define BACK 2
#define ERROR 3
#define BAD_UZ 4
#define BAD_IR 5
#define RIGHT 10
#define LEFT 100

/**
 *
 * @param Struktura
 * @info Pripojenie motora k MCU
 */
struct motor_control {
	uint8_t step;    // MCU pin pre krok
	uint8_t sm_[2];  // 0-sm0, 1-sm1
	uint8_t enable;  // MCU pin pre zapnutie
	uint8_t dir;     // MCU pin pre smer
	uint8_t reset;   // MUX pin pre reset
	uint8_t home;    // MUX pin pre informaciu o domacej pozicii
	uint8_t faul;    // MUX pin pre informaciu o prehriati
	int full_step; // pocet spravenych krokov
        uint16_t number_step; // pocet spravenych krokov
	bool    mux;     // stav INIT pinu MUX pre jeho funkcnost
	uint8_t mux_out; // vystup/vstup MUX
};

/**
 *
 * @param Struktura
 * @info informacie o kolese a motore
 */
struct motor_wheel {
    float r;            // priemer kolesa
    float step_360;   // pocet krokov pre celu otocku pri plnom kroku
    float d;           // polomer medzi kolesami
    float step_360_r; // pocet krokov pre celu otocku robota pri 2 motorovom otacani
};

/**
 *
 * @param Struktura
 * @info Pripojenie infra cervenych taktickych senzorov k MCU
 */
struct IR_proximity {
	bool mux;               // stav INIT pinu MUX pre jeho funkcnost
	uint8_t critical[2];    // MCU pin kritickych bodov pre prerusenie
	uint8_t comparator[2];  // MCU pin komparatorov
	uint8_t analog[2];      // MCU pin pre analogovu hodnotu
};

/**
 *
 * @param Struktura
 * @info Pripojenie ultrazvukoveho senzora k MCU
 */
struct ultrasonic {
	uint8_t echo;   // MCU pin echo
	uint8_t trig;   // MCU pin trigr
};

/**
 *
 * @param Struktura
 * @info X,Y,Z os pre magneticky senzor a akceleromter
 */
struct acc_mag {
	int16_t x;
	int16_t y;
	int16_t z;
};




	
#endif
