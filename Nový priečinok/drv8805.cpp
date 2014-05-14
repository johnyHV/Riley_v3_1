#include "drv8805.h"

drv8805::drv8805(motor_control *v_motor_0, motor_control *v_motor_1)
{
  motor_0 = v_motor_0;
  motor_1 = v_motor_1;
  
  motor_0->number_step = 0;
  motor_1->number_step = 0;
  
  instance = this;

}

drv8805::drv8805(motor_control *v_motor_0)
{
  motor_0 = v_motor_0;
  motor_0->number_step = 0;
  instance = this;
}

void drv8805::interrupt_motor_0()
{
    instance->ink_0();
}

void drv8805::ink_0()
{
      motor_0->number_step++;
}

void drv8805::interrupt_motor_1()
{
  instance->ink_0();
}

void drv8805::ink_1()
{
    motor_1->number_step++;
}

void drv8805::initial(mux4067 &mux,bool number_motor)
{
  //reset DRV8805
  mux.pin_select(motor_0->reset);
  digitalWrite(motor_0->mux_out,true);
  delayMicroseconds(5);
  
  //vynulovanie poctu krokov
  motor_0->number_step = 0;
  
  //vytvorenie prerusenia 
  attachInterrupt(motor_0->home,interrupt_motor_0,RISING);
  
  if (number_motor)
  {    
    // reset DRV8805
    mux.pin_select(motor_1->reset);
    digitalWrite(motor_1->mux_out,true);
    delayMicroseconds(5);
    
    // vynulovanie poctu krokov
    motor_1->number_step = 0;
  
    //vytvorenie prerusenia 
    attachInterrupt(motor_1->home,interrupt_motor_1,RISING);
  }
}

uint8_t drv8805::step(bool dir_0, bool dir_1, uint8_t time, bool enable_0, bool enable_1)
{
  // urcenie smeru
  digitalWrite(motor_0->dir,dir_0);
  digitalWrite(motor_1->dir,dir_1);
  
  // krok pre vybrany motor
  if (enable_0)
    digitalWrite(motor_0->step,HIGH);
		 
  if (enable_1)
    digitalWrite(motor_1->step,HIGH);
  
  //casovy interval za ktory mozem znova poslat hodnotu
  delayMicroseconds(3);
  
  if (enable_0)
    digitalWrite(motor_0->step,LOW);
		 
  if (enable_1)
    digitalWrite(motor_1->step,LOW);
}

uint8_t drv8805::step(bool dir, uint8_t time, bool enable)
{
  //nastavenie smeru
  digitalWrite(motor_0->dir,dir);
  
  // krok ak ma byt krok
  if (enable)
    digitalWrite(motor_0->step,HIGH);
  
  // cakanie pred dalsim prikazom
  delayMicroseconds(3);
  
  //znulovanie kroku
  if (enable)
    digitalWrite(motor_0->step,LOW);
}

void drv8805::set_mod(uint8_t mod)
{
  digitalWrite(motor_0->sm_[0],motor_mod[mod][0]);
  digitalWrite(motor_0->sm_[1],motor_mod[mod][1]);
}

void drv8805::set_enable(bool enable_0, bool enable_1)
{
  digitalWrite(motor_0->enable,enable_0);
  digitalWrite(motor_1->enable,enable_1);
}

void drv8805::set_enable(bool status)
{
  digitalWrite(motor_0->enable,status);
}

uint8_t drv8805::set_disable(mux4067 &mux)
{
  detachInterrupt(motor_0->home);
}
