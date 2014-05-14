#include "sensor_head.h"

/**
 *
 * @info vrati hodnotu vzdialenosti v zavislosti od IR a UZ. 
 * @info ak je vzdialenost rovnaka +-3cm, vrati priemer hodnot
 * @info ak je UZ v intervale, IR tiez, ale rozdiel je viac ako +-3cm, vrati 0
 * @info ak je UZ v intervale, ale IR nie je do 150cm , vrati 0
 * @param uz - ultra zvukovy senzor
 * @param IR - sharp infra cerveny senzor
 * @return float
 */

float head_distance(ultrasonic uz, uint8_t sharp) {
    
    float d_uz = ultrasonic_distance_(&uz);
    float d_ir = sharp_distance(sharp); 
   
    //Serial.print("Head UZ: "); Serial.println(d_uz);
    //Serial.print("Head IR: "); Serial.println(d_ir);
    
    if ((d_uz >= 15) && (d_uz <= 150)){						// kontrola vzdialenosti pre hranice sharp
        if (d_ir < 150) {
            if ((d_uz >= d_ir-20) && (d_uz <= d_ir+20)){
                return (d_uz + d_ir) /2;
            }
            else
            	return 0;
        }
        else 
        	return 0;
    }
    else 
    	return d_uz;
}



/**
 *
 * @info resetne pocet krokov pre motor hlavy
 * @param 
 * @param 
 * @return 
 */
void head_reset_step(motor_control *motor) {
    motor->number_step = 0;
}

/**
 *
 * @info resetne pocet krokov pre motor hlavy
 * @param 
 * @param 
 * @return 
 */
void head_reset_step(motor_control *motor_f,motor_control *motor_b){
    motor_f->number_step = 0;
    motor_b->number_step = 0;
}

/**
 *
 * @info otoci hlavu o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
uint16_t rotate_head_angle(uint8_t motor, float uhol, uint8_t smer) { 
    int kroky = (uhol / (360.0 / wheel.step_360));
    bool dir = false;
    if (smer == LEFT)
        dir = true;    // v pravo
    else
        dir = false;     // v lavo
    
    if (motor == FRONT) {
        motor_f.smer_otocenia = smer;
        motor_f.uhol_otocenia = uhol;
        motor_enable(&motor_f,true);
        
        for (uint16_t i=0;i<=kroky;i++){
            motor_step(&motor_f, &wheel, dir);
            delayMicroseconds(2000);
        }
        motor_enable(&motor_f,false);
    }
    else if (motor == BACK) {
        motor_b.smer_otocenia = smer;
        motor_b.uhol_otocenia = uhol;
        motor_enable(&motor_b,true);
        
        for (uint16_t i=0;i<=kroky;i++){
            motor_step(&motor_b, &wheel, dir);
            delayMicroseconds(2000);
        }
        motor_enable(&motor_b,false);
    }
    //return motor.number_step;
}

/**
 *
 * @info otoci obe hlavy o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
uint16_t rotate_head_angle(float uhol, uint8_t smer) {
    int kroky = (uhol / (360.0 / wheel.step_360));
    bool dir = false;
    
    if (smer == LEFT)
        dir = true;
    else
        dir = false;
        
    motor_enable(&motor_f, &motor_b, true, true);
    for (uint16_t n_steps = 0; n_steps <= kroky; n_steps++) {
        motor_step(&motor_f, &motor_b, &wheel, dir, dir);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_f, &motor_b, false, false);
    
    //return motor_f.number_step;
}

/**
 *
 * @info vycentruje snimaciu hlavu
 * @param 
 * @param 
 * @return 
 */
void head_center(motor_control motor, motor_wheel wheel, uint8_t smer) {

    bool dir = false;
    
    if (smer == LEFT)
        dir = true;    // v pravo
    else
        dir = false;     // v lavo
    
    motor_enable(&motor, true);
    
    uint16_t z_kroky = (uint16_t) motor.number_step;
    
    for (; z_kroky > 0;z_kroky--){
        motor_step(&motor, &wheel, dir);
        delayMicroseconds(2000);
    }    
    motor_enable(&motor, false);
}

/**
 *
 * @info vycentruje snimaciu hlavu
 * @param 
 * @param 
 * @return 
 */
void head_center(motor_control motor_f,motor_control motor_b, motor_wheel wheel, uint8_t smer) {
    bool dir = false;
    
    if (smer == LEFT)
        dir = true;    // v pravo
    else
        dir = false;     // v lavo
    
    uint16_t i= motor_f.number_step;
    motor_enable(&motor_f, &motor_b, true, true);
    for (; i> 0;i--){
        motor_step(&motor_f, &motor_b, &wheel, dir, dir);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_f, &motor_b, false, false);        
}