#include "drv8805_.h"

/**
 *
 * @info reset drv8805, inicializacia prerusenia, vynulovanie premennych, pripravenie na krok, false - inicializacia 0 motora. true - inicializacia 0,1 motora
 * @param Objekt multiplexora pre inicializaciu motora (reset)
 * @param Struktura s pripojenim motora k MCU
 * @return void
 */
void motor_initial(motor_control *motor)
{
	//inicializacia pinov
	pinMode(motor->enable,OUTPUT);
	pinMode(motor->step,OUTPUT);
	pinMode(motor->dir,OUTPUT);
	pinMode(motor->sm_[0],OUTPUT);
	pinMode(motor->sm_[1],OUTPUT);
	delayMicroseconds(100);
	
	//zapnutie
	digitalWrite(motor->enable,!true);
	digitalWrite(motor->step,true);
	
	//reset DRV8805
	mux.initial(motor->mux);
	mux.pin_select(motor->reset);
	delayMicroseconds(100);
	mux.pin_write(true);
	delayMicroseconds(100);
	mux.pin_write(false);
	
	// vynulovanie poctu krokov
	motor->number_step = 0;  
        motor->full_step = 0;
	mux.initial(false);
}

/**
 *
 * @info Dalsi krok pre 2 motory sucasne
 * @param motor_0 - pripojenie k MCU
 * @param motor_1 - pripojenie k MCU
 * @param dir_0 - smer jazdy 1 motora 1-pravo, 0-lavo
 * @param dir_1 - smer jazdy 2 motora 1-pravo, 0-lavo
 * @param time - casova medzera medi .... default 5
 * @param enable_0 - zapnutie 1 motora
 * @param enable_1 - zapnutie 2 motora
 * @return Pocet spravenych krokov
 */
int motor_step(motor_control *motor_0, motor_control *motor_1, motor_wheel * wheel,  bool dir_0, bool dir_1, uint8_t time, bool enable_0, bool enable_1)
{
	// urcenie smeru
	digitalWrite(motor_0->dir,dir_0);
	digitalWrite(motor_1->dir,dir_1);
  
	// krok pre vybrany motor
	digitalWrite(motor_0->step,HIGH);
        
        if (motor_0->number_step == wheel->step_360) {
            motor_0->full_step++;
            motor_0->number_step = 0;
        }
        
        if (motor_1->number_step == wheel->step_360) {
            motor_1->full_step++;
            motor_1->number_step = 0;
        }
        
	motor_0->number_step++;
	motor_1->number_step++;

	//casovy interval za ktory mozem znova poslat hodnotu
	delayMicroseconds(time);
	
	
	digitalWrite(motor_0->step,LOW);
}

/**
 *
 * @info Dalsi krok pre jeden motor
 * @param motor - pripojenie k MCU
 * @param dir - smer jazdy 1-pravo, 0-lavo
 * @param time - casova medzera medi .... default 5
 * @param enable - zapnutie motora
 * @return Pocet spravenych krokov
 */
int motor_step(motor_control *motor, motor_wheel * wheel, bool dir, uint8_t time, bool enable)
{
	//nastavenie smeru
	digitalWrite(motor->dir,dir);
  
	// krok ak ma byt krok
	digitalWrite(motor->step,HIGH);
        if (motor->number_step == wheel->step_360) {
            motor->full_step++;
            motor->number_step = 0;
        }
	motor->number_step++;
  
	// cakanie pred dalsim prikazom
	delayMicroseconds(time);
  
	//znulovanie kroku
	digitalWrite(motor->step,LOW);
    
	return motor->number_step;
}

/**
 *
 * @info Nastavenie modu spinania cievok
 * @param motor - pripojenie k MCU
 * @param mod - mod spinania cievok 0- 2 fazy proti sebe, 1-polovicny krok, 2- jedna faza
 * @return void
 */
void motor_set_mod(motor_control *motor, uint8_t mod)
{
    
	digitalWrite(motor->sm_[0],motor_mod[mod][0]);
	digitalWrite(motor->sm_[1],motor_mod[mod][1]);
}

/**
 *
 * @info Zapnutie 2 motorov
 * @param motor - pripojenie k MCU
 * @param enable - zapnutie 1 motora 1 - on
 * @return void
 */
void motor_enable(motor_control *motor,  bool enable)
{
	digitalWrite(motor->enable,!enable);
}

/**
 *
 * @info Zapnutie 2 motorov
 * @param motor_0 - pripojenie k MCU
 * @param motor_1 - pripojenie k MCU
 * @param enable_0 - zapnutie 1 motora 1 - on
 * @param enable_1 - zapnutie 2 motora 1 - on
 * @return void
 */
void motor_enable(motor_control *motor_0, motor_control *motor_1,bool enable0, bool enable1)
{
	digitalWrite(motor_0->enable,!enable0);
	digitalWrite(motor_1->enable,!enable1);
}

/**
 *
 * @info Resetnutie kontrolera, vynulovanie poctu krokov
 * @param mux - objekt pre multiplexor
 * @param motor - pripojenie motora k MCU
 * @return void
 */
void motor_reset(motor_control *motor)
{
	mux.initial(motor->mux);
	mux.pin_select(motor->reset);
	digitalWrite(motor->mux_out,true);
	delayMicroseconds(100);
	digitalWrite(motor->mux_out,false);
	
	// vynulovanie poctu krokov
	motor->number_step = 0; 
}

