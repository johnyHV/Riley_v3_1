#include "mux4067.h"

/**
 *
 * @info Inicializacia multiplexora
 * @param pole pinov A,B,C,D pripojenia k MCU
 * @param pin inicializcie pripojeny k MCU
 * @param pin vstup/vystup pripojeny k MCU
 * @return void
 */
mux4067::mux4067(uint8_t (&v_control_pin)[4], uint8_t v_initial, uint8_t v_output)
{
   for (uint8_t i=0; i<4; i++)
     control_pin[i] = v_control_pin[i];
   
   init = v_initial;
   outp = v_output;
   
   for (uint8_t i=0; i<4; i++)
    pinMode(control_pin[i], OUTPUT);
   
   pinMode(init, OUTPUT);
   pinMode(outp, OUTPUT);
   initial(false);
   pin_write(false);
   pin_select(0);
}
 
 /**
 *
 * @info Inicializacia INIT pinu multiplexora
 * @param status - stav inicializacie
 * @return void
 */
 void mux4067::initial(bool status)
 {
   digitalWrite(init, status);
 }
 
 /**
 *
 * @info Vycitanie aktualneho stavu inicializacie
 * @return digitalny vystup inicializacie
 */
 bool mux4067::initial_status()
 {
   return digitalRead(init);
 }
 
/**
 *
 * @info Nastavenie multiplexora pre dany pin
 * @param Cislo pinu
 * @return void
 */
 void mux4067::pin_select(uint8_t input)
 {
	for (uint8_t i=0; i<4;i++) {
		digitalWrite(control_pin[i],pin[input][i]);
	}
 }
 
/**
 *
 * @info Nastavenie statusu na vstup/vystup mux/demux
 * @param status
 * @return void
 */
 void mux4067::pin_write(bool status)
 {
   pinMode(outp,OUTPUT); 
   delayMicroseconds(100);
   digitalWrite(outp,status);
 }

/**
 *
 * @info Vycitanie stavu na vstupe/vystupe multiplexora
 * @return digitalna hodnota statusu
 */ 
 bool mux4067::pin_read()
 {
   pinMode(outp, INPUT);
   delayMicroseconds(1000);
   return digitalRead(outp);
 }
