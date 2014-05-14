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
   
    
    if ((d_uz >= 15) && (d_uz <= 150)){						// kontrola vzdialenosti pre hranice sharp
        if (d_ir < 150) {
            if ((d_uz >= d_ir-5) && (d_uz <= d_ir+5)){
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
 * @info otoci hlavu o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
void head_reset_step(motor_control motor) {
    motor.number_step == 0;
}

/**
 *
 * @info otoci hlavu o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
void head_reset_step(motor_control motor_f,motor_control motor_b){
    motor_f.number_step == 0;
    motor_b.number_step == 0;
}

/**
 *
 * @info otoci hlavu o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
uint16_t rotate_head_angle(motor_control motor, motor_wheel wheel, float uhol, uint8_t smer) { 
    int kroky = (uhol / (360.0 / wheel.step_360));
    motor_enable(&motor,true);
    bool dir = false;
    
    if (smer == LEFT)
        dir = false;    // v pravo
    else
        dir = true;     // v lavo
        
    
    for (uint16_t i=0;i<=kroky;i++){
        motor_step(&motor, &wheel, dir);
        delayMicroseconds(2000);

    }
    motor_enable(&motor,false);
    
    return motor.number_step;
                   
}

/**
 *
 * @info otoci obe hlavy o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
uint16_t rotate_head_angle(motor_control motor_f,motor_control motor_b, motor_wheel wheel, float uhol, uint8_t smer) {
    int kroky = (uhol / (360.0 / wheel.step_360));
    bool dir = false;
    
    if (smer == LEFT)
        dir = false;
    else
        dir = true;
        
    motor_enable(&motor_f, &motor_b, true, true);
    for (uint16_t n_steps = 0; n_steps <= kroky; n_steps++) {
        motor_step(&motor_f, &motor_b, &wheel, dir, dir);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_f, &motor_b, false, false);
    
    return motor_f.number_step;
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
        dir = false;    // v pravo
    else
        dir = true;     // v lavo
    
    motor_enable(&motor, true);
    
    Serial.print("Kroky motora: "); Serial.println(motor.number_step);
    uint16_t z_kroky = (uint16_t) motor.number_step;
    Serial.print("z_Kroky: "); Serial.println(z_kroky);
    
    for (; z_kroky > 0;z_kroky--){
        motor_step(&motor, &wheel, dir);
        delayMicroseconds(2000);
        Serial.print("z_kroky: "); Serial.println(z_kroky);
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
        dir = false;    // v pravo
    else
        dir = true;     // v lavo
    
    uint16_t i= motor_f.number_step;
    motor_enable(&motor_f, &motor_b, true, true);
    for (; i> 0;i--){
        motor_step(&motor_f, &motor_b, &wheel, dir, dir);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_f, &motor_b, false, false);        
}