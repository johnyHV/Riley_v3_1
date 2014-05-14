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

    if ((d_uz >= 15) && (d_uz <= 150)) { // kontrola vzdialenosti pre hranice sharp
        if (d_ir < 150) {
            if ((d_uz >= d_ir - 15) && (d_uz <= d_ir + 15)) {
                return (d_uz + d_ir) / 2;
            } else
                return 0;
        } else
            return d_uz;
    } else
        return d_uz;
}

/**
 *
 * @info resetne pocet krokov, plnych krokov, loop krokov
 * @param motor ktoreho parametre chcem resetnut
 * @return 
 */
void head_reset(motor_control *motor) {
    motor->full_step = 0;
    motor->l_kroky = 0;
    motor->l_full_step = 0;
    motor->number_step = 0;
}

/**
 *
 * @info resetne pocet krokov, plnych krokov, loop krokov
 * @param motor ktoreho parametre chcem resetnut
 * @param motor ktoreho parametre chcem resetnut
 * @return 
 */
void head_reset(motor_control *motor_0, motor_control * motor_1) {
    motor_0->full_step = 0;
    motor_0->l_kroky = 0;
    motor_0->l_full_step = 0;
    motor_0->number_step = 0;

    motor_1->full_step = 0;
    motor_1->l_full_step = 0;
    motor_1->l_kroky = 0;
    motor_1->number_step = 0;
}

/**
 *
 * @info resetne pocet krokov pre motor hlavy
 * @param motor ktoreho parametre chcem resetnut
 * @return 
 */
void head_reset_step(motor_control *motor) {
    motor->number_step = 0;
    motor->full_step = 0;
}

/**
 *
 * @info resetne pocet krokov pre motor hlavy
 * @param 
 * @param 
 * @return 
 */
void head_reset_step(motor_control *motor_f, motor_control *motor_b) {
    motor_f->number_step = 0;
    motor_b->number_step = 0;
}

/**
 *
 * @info otoci hlavu o vybrany uhol
 * @param motor FRONT,BACK
 * @param  uhol otocenia
 * @param smer otacania LEFT, RIGHT
 * @return 
 */
void rotate_head_angle(uint8_t motor, float uhol, uint8_t smer) {
    int kroky = (uhol / (360.0 / wheel.step_360));

    if (motor == FRONT) {
        motor_f.smer_otocenia = !smer;
        motor_f.uhol_otocenia = uhol;
        motor_f.l_dir = !smer;
        motor_f.l_kroky = kroky;
        motor_enable(&motor_f, true);
        motor_f.l_enable = true;
    } else if (motor == BACK) {
        motor_b.smer_otocenia = !smer;
        motor_b.uhol_otocenia = uhol;
        motor_b.l_dir = !smer;
        motor_b.l_kroky = kroky;
        motor_enable(&motor_b, true);
        motor_b.l_enable = true;
    }
}

/**
 * UPRAVIT
 * @info otoci obe hlavy o vybrany uhol
 * @param 
 * @param 
 * @return 
 */
uint16_t rotate_head_angle(float uhol, uint8_t smer) {
    int kroky = (uhol / (360.0 / wheel.step_360));
    
    motor_b.smer_otocenia = smer;
    motor_f.smer_otocenia = !smer;
    motor_b.uhol_otocenia = motor_f.uhol_otocenia = uhol;
    motor_f.l_dir = !smer;
    motor_f.l_kroky = kroky;
    motor_enable(&motor_f,&motor_b, true, true);
    motor_f.l_enable = true;
    
}

/**
 *
 * @info vycentruje snimaciu hlavu
 * @param 
 * @param 
 * @return 
 */
void head_center(uint8_t motor) {

    if (motor == FRONT) {
        head_reset_step(&motor_f);
        motor_f.l_dir = !motor_f.smer_otocenia;
        motor_f.smer_otocenia = CENTER;
        motor_f.uhol_otocenia = 0;
        motor_enable(&motor_f, true);
        motor_f.l_enable = true;
    } else if (motor == BACK) {
        head_reset_step(&motor_b);
        motor_b.l_dir = !motor_b.smer_otocenia;
        motor_b.smer_otocenia = CENTER;
        motor_b.uhol_otocenia = 0;
        motor_enable(&motor_b, true);
        motor_b.l_enable = true;
    }
}

/**
 * UPRAVIT
 * @info vycentruje snimaciu hlavu
 * @param 
 * @param 
 * @return 
 */
void head_center(motor_control motor_f, motor_control motor_b, motor_wheel wheel, uint8_t smer) {
    bool dir = false;

    if (smer == LEFT)
        dir = true; // v pravo
    else
        dir = false; // v lavo

    uint16_t i = motor_f.number_step;
    motor_enable(&motor_f, &motor_b, true, true);
    for (; i > 0; i--) {
        motor_step(&motor_f, &motor_b, &wheel, dir, dir);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_f, &motor_b, false, false);
}