#include "premenne.h"

// <<<<<<<<<<<<<<<< pripojenie HW k MCU >>>>>>>>>>>>>>>>
// pin MUX
uint8_t mux_control_pin[4] = {36,37,38,39};                     // MUX - pripojenie A,B,C,D
uint8_t mux_initial = 40;                                       // MUX - pripojenie init pinu
uint8_t mux_output = 43;                                        // MUX - pripojenie IN/OUT pinu
mux4067 mux = mux4067(mux_control_pin,mux_initial,mux_output);  // Vytvorenie objektu MUX

// IR senzory
//mux,critical,komparator,analog
IR_proximity proximity = {false,{2,3},{41,42},{2,3},{12,13,15}};           // pripojenie proximitno reflexnych senzorov
uint8_t sharp_f = 4;                                      // pripojenie Sharp predneho senzoru
uint8_t sharp_b = 5;                                      // pripojenie Sharp zadneho senzoru

//ultrazvuk
ultrasonic uz_f = {35,34};                                      // Predny ultrazvukovy senzor
ultrasonic uz_b = {33,32};                                      // Zadny ultrazvukovy senzor

// krokace
//step,sm,enable,dir,reset,home,faul,step_number,mux,mux_out
motor_control motor_l = {31,{26,27},22,28,0,50,2,0,true,43,false,false,0,0,CENTER,0};    // Krokovy motor lavy
motor_control motor_r = {31,{26,27},23,29,3,52,5,0,true,43,false,false,0,0,CENTER,0};    // Krokovy motor prav
motor_control motor_f = {31,{26,27},24,30,12,52,8,0,true,43,false,false,0,0,CENTER,0};   // krokovy motor predny
motor_control motor_b = {31,{26,27},25,30,14,52,11,0,true,43,false,false,0,0,CENTER,0};   // krokovy motor zadny
bool b_mod[4] = {false,false,false,false};                     	// zap/vyp,smer,zap/vyp,smer krokovych motorv
bool kontrola = true;                                           // premenna pre funkciu hladaj_sever
motor_wheel wheel = {3.3,2048.0,26.0,8400.0,20};                // specifikacia kolesa a motora


// <<<<<<<<<<<<<<<< Konstanty>>>>>>>>>>>>>>>>
uint8_t v_stena = 30; // konstantna vzdialenost od steny. realne 15cm