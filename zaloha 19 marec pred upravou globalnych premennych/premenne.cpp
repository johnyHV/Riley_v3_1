#include "premenne.h"

// <<<<<<<<<<<<<<<< pripojenie HW k MCU >>>>>>>>>>>>>>>>
// pin MUX
uint8_t mux_control_pin[4] = {36,37,38,39};                     // MUX - pripojenie A,B,C,D
uint8_t mux_initial = 40;                                       // MUX - pripojenie init pinu
uint8_t mux_output = 43;                                        // MUX - pripojenie IN/OUT pinu
mux4067 mux = mux4067(mux_control_pin,mux_initial,mux_output);            // Vytvorenie objektu MUX


// IR senzory
//mux,critical,komparator,analog
IR_proximity proximity = {false,{2,3},{41,42},{2,3}};           // pripojenie proximitno reflexnych senzorov
uint8_t sharp_f = 4;                                      // pripojenie Sharp predneho senzoru
uint8_t sharp_b = 5;                                      // pripojenie Sharp zadneho senzoru

//ultrazvuk
ultrasonic uz_f = {35,34};                                      // Predny ultrazvukovy senzor
ultrasonic uz_b = {33,32};                                      // Zadny ultrazvukovy senzor
