#ifndef _premenne_h                                                   
#define _premenne_h

#include <Arduino.h>
#include "mux4067.h"
//#include "LM75.h"
#define FRONT 100
#define BACK 200
#define ERROR 3
#define BAD_UZ 4
#define BAD_IR 5
#define RIGHT 1
#define LEFT 0
#define CENTER 70

/**
 *
 * @param Struktura
 * @info Pripojenie motora k MCU
 */
struct motor_control {
    uint8_t step; // MCU pin pre krok
    uint8_t sm_[2]; // 0-sm0, 1-sm1
    uint8_t enable; // MCU pin pre zapnutie
    uint8_t dir; // MCU pin pre smer
    uint8_t reset; // MUX pin pre reset
    uint8_t home; // MUX pin pre informaciu o domacej pozicii
    uint8_t faul; // MUX pin pre informaciu o prehriati
    int full_step; // pocet spravenych krokov
    uint16_t number_step; // pocet spravenych krokov
    bool mux; // stav INIT pinu MUX pre jeho funkcnost
    uint8_t mux_out; // vystup/vstup MUX
    bool l_enable; // zapnutie alebo vypnutie motora pre slucku
    bool l_dir; // smer motora pre slucku
    uint16_t l_kroky; // pocet krokov kolko treba spravit pre slucku
    uint16_t l_full_step; //pocet otociek kolesa pre slucku    
    uint8_t smer_otocenia; // smer posledneho otacania motora. true - LEFT, false - RIGHT, center
    uint8_t uhol_otocenia; // uhol otocenia
};

/**
 *
 * @param Struktura
 * @info informacie o kolese a motore
 */
struct motor_wheel {
    float r; // priemer kolesa
    float step_360; // pocet krokov pre celu otocku pri plnom kroku
    float d; // polomer medzi kolesami
    float step_360_r; // pocet krokov pre celu otocku robota pri 2 motorovom otacani
    uint8_t r_k; // konstanta o ktoru sa zvecsi vzdialenost z hlavy po otoceni
};

/**
 *
 * @param Struktura
 * @info Pripojenie infra cervenych taktickych senzorov k MCU
 */
struct IR_proximity {
    bool mux; // stav INIT pinu MUX pre jeho funkcnost
    uint8_t critical[2]; // MCU pin kritickych bodov pre prerusenie
    uint8_t comparator[2]; // MCU pin komparatorov
    uint8_t analog[2]; // MCU pin pre analogovu hodnotu
    uint8_t schody[3]; // pripojenie snimacov schodov
};

/**
 *
 * @param Struktura
 * @info Pripojenie ultrazvukoveho senzora k MCU
 */
struct ultrasonic {
    uint8_t echo; // MCU pin echo
    uint8_t trig; // MCU pin trigr
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

// <<<<<<<<<<<<<<<< pripojenie HW k MCU >>>>>>>>>>>>>>>>
// pin MUX
extern uint8_t mux_control_pin[4]; // MUX - pripojenie A,B,C,D
extern uint8_t mux_initial; // MUX - pripojenie init pinu
extern uint8_t mux_output; // MUX - pripojenie IN/OUT pinu
extern mux4067 mux; // Vytvorenie objektu MUX

// IR senzory
//mux,critical,komparator,analog
extern IR_proximity proximity; // pripojenie proximitno reflexnych senzorov
extern uint8_t sharp_f; // pripojenie Sharp predneho senzoru
extern uint8_t sharp_b; // pripojenie Sharp zadneho senzoru

//ultrazvuk
extern ultrasonic uz_f; // Predny ultrazvukovy senzor
extern ultrasonic uz_b; // Zadny ultrazvukovy senzor

// krokace
//step,sm,enable,dir,reset,home,faul,step_number,mux,mux_out
extern motor_control motor_l; // Krokovy motor lavy
extern motor_control motor_r; // Krokovy motor prav
extern motor_control motor_f; // krokovy motor predny
extern motor_control motor_b; // krokovy motor zadny
extern bool b_mod[4]; // zap/vyp,smer,zap/vyp,smer krokovych motorv
extern bool kontrola; // premenna pre funkciu hladaj_sever
extern motor_wheel wheel; // specifikacia kolesa a motora

// <<<<<<<<<<<<<<<< Konstanty>>>>>>>>>>>>>>>>
extern uint8_t v_stena; // konstantna vzdialenost od steny. realne 15cm

#endif
