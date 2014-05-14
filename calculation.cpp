#include "calculation.h"

/**
 *
 * @info Vrati prejdenu vzdialenost daneho motora v cm
 * @param motor - pripojenie motora k MCU
 * @return float
 */
float motor_distance(motor_control *motor) {


    /*int cele_otacky = ((int) motor->number_step / wheel->step_360);
    float cela_vzdialenost = 2 * 3.14 * wheel->r * cele_otacky;

    int zvysok_krokov = ((int) motor->number_step % wheel->step_360);
    float zvysna_vzdialenost = (2 * 3.14 * wheel->r * (wheel->step_360 / 360) * zvysok_krokov) / 360;

    return cela_vzdialenost + zvysna_vzdialenost;
     */

    float cela_vzdialenost = 2.0 * 3.14 * (float) wheel.r * (float) motor->full_step;

    //uint16_t zvysok_krokov = ((int) motor->number_step % (int) wheel.step_360);
    float zvysna_vzdialenost = (2.0 * 3.14 * wheel.r * (( 360.0 / wheel.step_360) * (float) motor->number_step)) / 360.0;

    return cela_vzdialenost + zvysna_vzdialenost;
}

/**
 *
 * @info Vrati uhol otocenia krokoveho motora
 * @param motor - pripojenie motora k MCU
 * @return float
 */
float angle_twist(motor_control *motor, motor_wheel * wheel) {
    int zvysok_krokov = ((int) motor->number_step % (int) wheel->step_360);
    return (360 / wheel->step_360) * zvysok_krokov;
}

/**
 *
 * @info Vypocita pocet krokov na zaklade vzdialenosti
 * @param float distance - vzdialenost
 * @param motor motor - motor
 * @return float
 */
void calc_step_of_distance(float distance, motor_control *motor) {
    motor->full_step = 0;
    motor->l_kroky = 0;
    motor->l_full_step = 0;
    motor->number_step = 0;
    
    int kroky = distance / ((2.0 * 3.14 * (float) wheel.r) / wheel.step_360);
    motor->l_kroky = kroky % (int) wheel.step_360;
    motor->l_full_step = kroky / wheel.step_360;
}

/**
 *
 * @info Zisti realnu hodnotu vzdialenosti pri nezhode UZ a SHARP senzoru
 * @param bool smer - smer jazdy 
 * @return float
 */
float senzor_check(uint8_t smer) {

    Serial.println("start kontroly");
    float f_sharp_d = sharp_distance(sharp_f); // vzdialenost v povodnej polohe z sharp
    float f_uz_d = ultrasonic_distance_(&uz_f); // vzdialenost v povodnej polohe z UZ
    float back_distance = head_distance(uz_b, sharp_b); // vzdialenost zadnej snimacej hlavy v povodnej polohe
    uint16_t d_back = 0;
    float f_back = 0.0;

    if (back_distance == 0.0)
        back_distance = ultrasonic_distance_(&uz_b);

    Serial.print("f_sharp: ");
    Serial.println(f_sharp_d);
    Serial.print("f_uz: ");
    Serial.println(f_uz_d);
    Serial.print("Back distance: ");
    Serial.println(back_distance);

    // pokial je za robotom miesto viac ako 7cm, vycuva 5cm, pokial nie vycuva 80% vzdialenosti
    if (back_distance > 7) {
        d_back = (((360.0 * 5.0) / (2.0 * 3.14 * 3.3)) / (360.0 / 2048.0)); // 0.17578125 = 360/2048            
        f_back = 5.0;
    } else if (back_distance > 1) {
        d_back = ((360.0 * ((back_distance / 100.0)*80.0)) / 2.0 * 3.14 * 3.3) / (360.0 / 2048.0); // 0.17578125 = 360/2048
        f_back = (back_distance / 100.0)*80.0;
    }
    Serial.print("d_back: ");
    Serial.println(d_back);
    bool critical = false; // urcenie prekazky v kritickych bodoch
    // kontrola ci nie je prekazka za robotom
    /*for (uint8_t sensor = 7; sensor <= 10; sensor++) {
        uint8_t o_proximity = proximity_d_status(&proximity, sensor, &mux, 3);
        if (o_proximity == ERROR) {
            // pokial je nezhoda komparatorov kritickych bodov
        } else {
            if (o_proximity != false) {
                critical = true;
                sensor = 11;
            }
        }
    }*/

    if (critical == false) {
        motor_enable(&motor_l, &motor_r, true, true);
        for (uint16_t n_steps = 0; n_steps <= d_back; n_steps++) {
            motor_step(&motor_l, &motor_r, &wheel, false, true);
            delayMicroseconds(2000);
        }
        motor_enable(&motor_l, &motor_r, false, false);
    }

    float n_f_sharp_d = sharp_distance(sharp_f);
    float n_f_uz_d = ultrasonic_distance_(&uz_f);
    float n_back_distance = head_distance(uz_b, sharp_b);
    bool IR = false;
    bool UZ = false;
    bool BACK_ = false;

    Serial.print("f_sharp: ");
    Serial.println(n_f_sharp_d);
    Serial.print("f_uz: ");
    Serial.println(n_f_uz_d);
    Serial.print("Back distance: ");
    Serial.println(n_back_distance);

    // vrati sa na podovne miesto
    motor_enable(&motor_l, &motor_r, true, true);
    for (uint16_t n_steps = 0; n_steps <= d_back; n_steps++) {
        motor_step(&motor_l, &motor_r, &wheel, true, false);
        delayMicroseconds(2000);
    }
    motor_enable(&motor_l, &motor_r, false, false);

    // kontrola spravnosti UZ
    float rozdiel_uz = n_f_uz_d - f_uz_d - f_back;
    Serial.print("rozdiel UZ: ");
    Serial.println(rozdiel_uz);
    if ((rozdiel_uz > -2.5) && (rozdiel_uz < 2.5)) {
        UZ = true;
        Serial.println("Spravny UZ");
    }


    // kontrola spravnosti IR
    float rozdiel_ir = n_f_sharp_d - f_sharp_d - f_back;
    Serial.print("rozdiel IR: ");
    Serial.println(rozdiel_ir);
    if ((rozdiel_ir > -2.5) && (rozdiel_ir < 2.5)) {
        Serial.println("Spravny IR");
        IR = true;
    }


    // kontrola ci realne vycuval cca 5cm
    float rozdiel_back = back_distance - n_back_distance - f_back;
    Serial.print("rozdiel v vycuvani: ");
    Serial.println(rozdiel_back);
    if ((rozdiel_back > -2.5) && (rozdiel_back < 2.5)) {
        Serial.println("Vycuval 5cm");
        BACK_ = true;
    }


    if (UZ && IR) {
        Serial.println("UZ a IR");
        return (f_uz_d + f_sharp_d) / 2;
    }

    if (UZ) {
        Serial.println("UZ");
        return f_uz_d;
    }

    if (IR) {
        Serial.println("IR");
        return f_sharp_d;
    }

}
/**
 *
 * @info Otoci robota o pozadovany uhol
 * @param float uhol - uhol o aky sa ma robot otocit
 * @param int smer - smer otocenia
 * @return float
 */
void otocenie_robota(float uhol, uint8_t smer) {

    float obvod = (3.14 * wheel.d * uhol) / 360.0;
    int kroky = (obvod / (2.0 * 3.14 * wheel.r)) * wheel.step_360;
    Serial.println(kroky);
    motor_l.l_dir = smer;
    motor_r.l_dir = smer;
    motor_l.full_step = 0;
    motor_r.full_step = 0;
    motor_l.number_step = 0;
    motor_r.number_step = 0;
    motor_l.l_kroky = kroky % (int) wheel.step_360;
    motor_r.l_kroky = kroky % (int) wheel.step_360;
    motor_l.l_full_step = kroky / (int) wheel.step_360;
    motor_r.l_full_step = kroky / (int) wheel.step_360;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;
}
/**
 *
 * @info Otoci robota o pozadovany uhol, a kontroluje ci v zadna strana nepadne z schodov
 * @param float uhol - uhol o aky sa ma robot otocit
 * @param int smer - smer otocenia
 * @return float
 */
void otocenie_robota_k(float uhol, uint8_t smer) {

    float obvod = (3.14 * wheel.d * uhol) / 360.0;
    int kroky = (obvod / (2.0 * 3.14 * wheel.r)) * wheel.step_360;
    Serial.println(kroky);
    motor_l.l_dir = smer;
    motor_r.l_dir = smer;
    motor_l.full_step = 0;
    motor_r.full_step = 0;
    motor_l.number_step = 0;
    motor_r.number_step = 0;
    motor_l.l_kroky = kroky % (int) wheel.step_360;
    motor_r.l_kroky = kroky % (int) wheel.step_360;
    motor_l.l_full_step = kroky / (int) wheel.step_360;
    motor_r.l_full_step = kroky / (int) wheel.step_360;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;

    while (motor_l.l_enable) {
        if (schody_back()) {
            prekazka_vzadu();
        }
        delay(11);
    }
}

/**
 *
 * @info vyhne sa prekazke
 * @param smer - smer kde je stena
 * @return voidz
 */
void prekazka(uint8_t smer) { // smer kde je stena. ak je v lavo, otoci sa robot v pravo
    Serial.println("Detekcia prekazky");

    head_reset(&motor_f, &motor_b);
    otocenie_robota(90, !smer);
    rotate_head_angle(FRONT, 90, smer);
    while (motor_f.l_enable)
        delay(11);

    rotate_head_angle(BACK, 90, !smer);
    while (motor_l.l_enable)
        delay(11);

    uint8_t front = head_distance(uz_f, sharp_f);
    uint8_t back = head_distance(uz_b, sharp_b);
    Serial.print("Front: ");
    Serial.println(front);
    Serial.print("Back: ");
    Serial.println(back);

    if (((front - back) < 10) && ((front - back) > -10)) { // zisti, ci prekazka je sirsia ako on sam. ci predny senzor uz nie je mimo prekazku
        front = 0;
    }

    Serial.println("Hodnoty su rovnake prekazka nie je sirsia: ");
    uint8_t stav = 1;
    bool b_stav = true;
    uint8_t n_back = 0;
    uint8_t n_front = 0;

    do {
        front = head_distance(uz_f, sharp_f);
        back = head_distance(uz_b, sharp_b);
        if (front == 0)
            front = ultrasonic_distance_(&uz_f);
        if (back == 0)
            back == ultrasonic_distance_(&uz_b);

        Serial.print("Front: ");
        Serial.println(front);
        Serial.print("Back: ");
        Serial.println(back);

        head_reset(&motor_l); // zapnem motory
        head_reset(&motor_r);
        motor_l.l_dir = true;
        motor_r.l_dir = false;
        motor_enable(&motor_l, &motor_r, true, true);
        motor_l.l_enable = true;
        motor_r.l_enable = true;

        do { // zistujem ci uz niesom mimo prekazku
            if (proximity_critical_status(&proximity)) { // zistuje ci v smere jazdy nema prekazku
                Serial.println("Kriticke!");
                b_stav = false;
                stav = 2;
            }

            n_front = head_distance(uz_f, sharp_f);
            n_back = head_distance(uz_b, sharp_b);
            if (n_front == 0)
                n_front = ultrasonic_distance_(&uz_f);
            if (n_back == 0)
                n_back == ultrasonic_distance_(&uz_b);

            Serial.print("n Front: ");
            Serial.println(n_front);
            Serial.print("n Back: ");
            Serial.println(n_back);
            delay(200);

            if (!((n_front < front + 10)&&(n_front > front - 10))) {
                Serial.println("Splnena prva");

                if (!((n_back < back + 10)&&(n_back > back - 10))) {
                    Serial.println("Splnena druha");
                    b_stav = false;
                }
            }

        } while (b_stav);

        motor_l.l_enable = false; // vypnem motory
        motor_r.l_enable = false;
        motor_enable(&motor_l, &motor_r, false, false);
        if (stav == 1) {
            otocenie_robota(90, smer);
            while (motor_l.l_enable)
                delay(11);
            stav = 2;
            b_stav = true;
        }
    } while (b_stav);

    head_center(FRONT);
    while (motor_f.l_enable)
        delay(11);

    head_center(BACK);
    while (motor_b.l_enable)
        delay(11);

}

/**
 *
 * @info odide od prekazky o 13cm
 * @param uint8_t smer - smer v ktorom je stena
 * @return void
 */
void vzdialenost_stena(uint8_t smer) {
    Serial.println("Otacam robota");

    if (motor_f.smer_otocenia == CENTER) { // pokial je to center, otoci sa k stene
        head_reset(&motor_f);
        rotate_head_angle(FRONT, 90, smer);
    } else if (motor_f.smer_otocenia != smer) {
        head_center(FRONT);
        head_reset(&motor_f);
        rotate_head_angle(FRONT, 90, smer);
    }

    while (motor_f.l_enable)
        delay(11);

    uint8_t front = head_distance(uz_f, sharp_f); // zisti vzdialenost steny

    if (front == 0)
        front = ultrasonic_distance_(&uz_f);

    Serial.print("Vzdialenost: ");
    Serial.println(front);
    if (front < v_stena) { // ak je do 13cm, odide od steny na 13cm.
        Serial.println("Vzdialenost do 13cm");
        float odstup = v_stena - front;
        float obvod = 2.0 * 3.14 * wheel.r;
        float uhol = RadiansToDegrees(atan2(odstup, obvod));

        otocenie_robota(uhol, !smer); // otoci robota o vypocitany uhol
        while (motor_l.l_enable)
            delay(11);

        head_reset(&motor_l);
        head_reset(&motor_r);
        motor_l.l_kroky = wheel.step_360 - 1;
        motor_r.l_kroky = wheel.step_360 - 1;
        motor_l.l_dir = true;
        motor_r.l_dir = false;
        motor_enable(&motor_l, &motor_r, true, true);
        motor_l.l_enable = true;
        motor_r.l_enable = true;

        while (motor_l.l_enable) // ide vpred o jednu otocku kolesa
            delay(11);

        otocenie_robota(uhol, smer);

        while (motor_l.l_enable) // otoci robota naspet o dany uhol
            delay(11);
    } else if (front > v_stena) { // ak je nad 13cm, pride k stene na 13cm
        Serial.println("Vzdialenost nad 15cm");
        float odstup = front - v_stena;
        float obvod = 2.0 * 3.14 * wheel.r;
        float uhol = RadiansToDegrees(atan2(odstup, obvod));

        otocenie_robota(uhol, smer);
        while (motor_l.l_enable)
            delay(11);

        head_reset(&motor_l);
        head_reset(&motor_r);
        motor_l.l_kroky = wheel.step_360 - 1;
        motor_r.l_kroky = wheel.step_360 - 1;
        motor_l.l_dir = true;
        motor_r.l_dir = false;
        motor_enable(&motor_l, &motor_r, true, true);
        motor_l.l_enable = true;
        motor_r.l_enable = true;

        while (motor_l.l_enable) // ide vpred o jednu otocku kolesa
            delay(11);

        otocenie_robota(uhol, !smer);
        while (motor_l.l_enable) // otoci robota naspet o dany uhol
            delay(11);
    }
    head_center(FRONT);
    while (motor_f.l_enable)
        delay(11);
}

/**
 *
 * @info odide od prekazky o x cm
 * @param uint8_t smer - smer v ktorom je stena
 * @param uint8_t vzdialenost - vzdialenost o aku sa priblizi k stene
 * @return void
 */
void vzdialenost_stena(uint8_t smer, uint8_t vzdialenost) {
    Serial.print("Otacam robota o vzdialenost: ");
    Serial.println(vzdialenost);

    float obvod = 2.0 * 3.14 * wheel.r;
    float uhol = RadiansToDegrees(atan2(vzdialenost, 2.0 * obvod));

    otocenie_robota(uhol, smer); // otoci robota o vypocitany uhol
    while (motor_l.l_enable)
        delay(11);

    head_reset(&motor_l);
    head_reset(&motor_r);
    motor_l.l_full_step = 1;
    motor_r.l_full_step = 1;
    motor_l.l_kroky = wheel.step_360 - 1;
    motor_r.l_kroky = wheel.step_360 - 1;
    motor_l.l_dir = true;
    motor_r.l_dir = false;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;

    while (motor_l.l_enable) // ide vpred o jednu otocku kolesa
        delay(11);

    otocenie_robota(uhol, !smer);

    while (motor_l.l_enable) // otoci robota naspet o dany uhol
        delay(11);
}

/**
 *
 * @info vyhne sa prekazke
 * @param smer - smer kde je stena
 * @return void
 */
bool test(uint8_t smer) {
    Serial.println("Detekcia prekazky");
    Serial.print("Proximity 4: ");
    Serial.println(proximity_a_status(&proximity, 4, mux));
    Serial.print("Proximity 5: ");
    Serial.println(proximity_a_status(&proximity, 5, mux));

    uint8_t h_distance = 0;
    uint8_t front = head_distance(uz_f, sharp_f); // hodnota v predu pred otocenim
    head_reset(&motor_f, &motor_b);
    otocenie_robota(90, !smer);
    rotate_head_angle(FRONT, 90, smer);
    while (motor_f.l_enable)
        delay(11);

    rotate_head_angle(BACK, 90, !smer);
    while (motor_l.l_enable)
        delay(11);

    uint8_t r_front = head_distance(uz_f, sharp_f); // prvotne hodnoty v predu
    uint8_t r_back = head_distance(uz_b, sharp_b); // prvotne hodnoty v zadu
    uint8_t stav = 1;

    Serial.print("Front: ");
    Serial.println(front);
    Serial.print("r_Front: ");
    Serial.println(r_front);
    Serial.print("r_Back: ");
    Serial.println(r_back);

    if (((r_back - front) > wheel.r_k) || (r_back == 0)) { // zisti ci tam nie je hranol
        Serial.println("Je hranol");
        stav = 3;
        r_front = 0;
    }

    if ((!(((r_front - r_back) < 10) && ((r_front - r_back) > -10)))&&(stav != 3)) { // zisti, ci prekazka je sirsia ako on sam. ci predny senzor uz nie je mimo prekazku
        Serial.println("Prekazka je kratsia");
        r_front = 0;
    }

    uint8_t n_back = 0;
    uint8_t n_front = 0;
    bool b_stav = true;

    do {
        head_reset(&motor_l); // zapnem motory
        head_reset(&motor_r);
        motor_l.l_dir = true;
        motor_r.l_dir = false;
        motor_enable(&motor_l, &motor_r, true, true);
        motor_l.l_enable = true;
        motor_r.l_enable = true;

        uint8_t p_back;
        do { // zistujem ci uz niesom mimo prekazku
            if (proximity_critical_status(&proximity)) { // zistuje ci v smere jazdy nema prekazku
                Serial.println("Kriticke!");
                //b_stav = false;
                //stav = 2;
            } else { // ak je prekazka, znamena ze to bud nebola prekazka ale koniec miestnosti, alebo dalsia prekazka

            }
            p_back = n_back;
            n_front = head_distance(uz_f, sharp_f);
            n_back = head_distance(uz_b, sharp_b);
            if (n_front == 0)
                n_front = sharp_distance(sharp_f);
            if (n_back == 0) {
                n_back == sharp_distance(sharp_b);
                //Serial.print("Chybny zadny koreckai IR: ");
                //Serial.println(n_back);
            }
            if (n_back == 0) {
                n_back == ultrasonic_distance_(&uz_b);
                //Serial.print("Chybny zadny koreckai UZ: ");
                //Serial.println(n_back);
            }
            if (n_back == 0) {
                n_back = p_back;
            }

            Serial.print("n Front: ");
            Serial.println(n_front);
            Serial.print("n Back: ");
            Serial.println(n_back);
            delay(200);

            if ((n_front > r_front + 10)&&(stav == 1)) { // status prvy, obidenie prekazky, zisenie konca prekazky
                Serial.println("Splnena prva, STAV-1");
                if (n_back > r_back + 10) {
                    Serial.println("Splnena druha, STAV-1");
                    b_stav = false;
                }
            } else if ((n_front < r_front - 10)&&(stav == 2)) { // status 2, najdenie zaciatku prekazky
                Serial.println("Splnena prva, STAV-2");
                if (n_back < r_back - 10) {
                    Serial.println("Splnena druha, STAV-2");
                    b_stav = false;
                }
            } else if ((n_back < front + wheel.r_k)&& (stav == 3)) { // najde zaciatok hrany zadnou hlavou
                Serial.println("Najdeny zaciatok hranola, STAV-3");
                b_stav = false;
            } else if ((n_back > front + wheel.r_k)&&(stav == 4)) { // najde koniec hranola zadnou hlavou
                Serial.println("Najdeny koniec hranola, STAV-4");
                b_stav = false;
            } else if ((n_back < r_back - 20)&&(stav == 5)) { // najdenie zaciatku hranola pri otoceni 90
                Serial.println("Najdeny zaciatok hranola, STAV-5");
                b_stav = false;
            } else if ((n_back > r_back + 3)&&(stav == 6)) { // najdenie konca hranola pri otoceni 90
                b_stav = false;
                Serial.println("Najdeny koniec hranola, STAV-6");
            }
        } while (b_stav);
        motor_l.l_enable = false; // vypnem motory
        motor_r.l_enable = false;
        motor_enable(&motor_l, &motor_r, false, false);

        if (stav == 1) { // otocenie okolo steny
            Serial.println("Stav 1");
            stav = 2;
            head_reset(&motor_l, &motor_r);
            otocenie_robota(90, smer);
            while (motor_l.l_enable)
                delay(11);
            r_front = head_distance(uz_f, sharp_f);
            r_back = head_distance(uz_b, sharp_b);
            if (r_front == 0)
                r_front = sharp_distance(sharp_f);
            if (r_back == 0)
                r_back = sharp_distance(sharp_b);
            b_stav = true;
        } else if (stav == 2) { // uz je otoceny ukoncuje sa algoritmus
            Serial.println("Stav 2");
            b_stav = false;
        } else if (stav == 3) {
            Serial.println("Stav 3");
            stav = 4;
            r_front = 0;
            r_back = head_distance(uz_b, sharp_b);
            if (r_back == 0)
                r_back = sharp_distance(sharp_b);
            b_stav = true;
        } else if (stav == 4) {
            Serial.println("Stav 4");
            h_distance = motor_distance(&motor_l);
            Serial.print("Hranol obisiel na: ");
            Serial.println(h_distance);
            stav = 5;
            head_reset(&motor_l, &motor_r);
            otocenie_robota(90, smer);
            while (motor_l.l_enable)
                delay(11);
            r_front = 0;
            //r_back = head_distance(uz_b, sharp_b);
            r_back = sharp_distance(sharp_b);
            b_stav = true;
        } else if (stav == 5) {
            Serial.println("Stav 5");
            stav = 6;
            r_front = 0;
            r_back = head_distance(uz_b, sharp_b);
            if (r_back == 0)
                r_back = sharp_distance(sharp_b);
            b_stav = true;
        } else if (stav == 6) {
            Serial.println("Stav 6");
            vzdialenost_stena(smer, h_distance + 28);
            b_stav = false;
        } else {
            Serial.println("Ziaden stav");
        }
        
        Serial.print("r_Front: ");
        Serial.println(r_front);
        Serial.print("r_Back: ");
        Serial.println(r_back);
    } while (b_stav);

    Serial.println("Koniec");
    head_center(FRONT);
    while (motor_f.l_enable)
        delay(11);

    head_center(BACK);
    while (motor_b.l_enable)
        delay(11);

    return true;
}


/**
 *
 * @info Zapne motory pre smer pohybu vpred s resetom informacii 
 * @return void
 */
void motor_on_front() {
    head_reset(&motor_l);
    head_reset(&motor_r);
    motor_l.l_dir = true;
    motor_r.l_dir = false;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;
}

/**
 *
 * @info Zapne motory pre smer pohybu vpred bez resetu informacii
 * @return void
 */
void motor_on_front_nr() {
    motor_l.l_dir = true;
    motor_r.l_dir = false;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;
}

/**
 *
 * @info zapne motory pre smer pohybu smer vzad s resetom informacii 
 * @return void
 */
void motor_on_back() {
    head_reset(&motor_l);
    head_reset(&motor_r);
    motor_l.l_dir = false;
    motor_r.l_dir = true;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;
}

/**
 *
 * @info zapne motory pre smer pohybu smer vzad bez resetu informacii
 * @return void
 */
void motor_on_back_nr() {
    motor_l.l_dir = false;
    motor_r.l_dir = true;
    motor_enable(&motor_l, &motor_r, true, true);
    motor_l.l_enable = true;
    motor_r.l_enable = true;
}

/**
 *
 * @info vypne motory
 * @return void
 */
void motor_off() {
    motor_l.l_enable = false; // vypnem motory
    motor_r.l_enable = false;
    motor_enable(&motor_l, &motor_r, false, false);
}

/**
 *
 * @info Vycuva x cm
 * @param distance - vzdialenost vycuvania
 * @return void
 */
void back_cm(float distance) {
    digitalWrite(6,HIGH);
    head_reset_step(&motor_l, &motor_r);
    calc_step_of_distance(distance, &motor_l);
    motor_on_back_nr();
    while (motor_l.l_enable)
        delay(11);
    digitalWrite(6,LOW);
}

/**
 *
 * @info Prejde vpred x cm
 * @param distance - vzdialenost prejdenia
 * @return void
 */
void front_cm(float distance) {
    head_reset_step(&motor_l, &motor_r);
    calc_step_of_distance(distance, &motor_l);
    motor_on_front_nr();
    while (motor_l.l_enable)
        delay(11);
}

void front_head_scan() {
    digitalWrite(5,HIGH);
    head_reset(&motor_f);
    
    rotate_head_angle(FRONT, 90, LEFT);
    rotate_head_angle(BACK, 90, RIGHT);
    while (motor_f.l_enable)
        delay(11);
    
    delay(100);
    
    head_reset(&motor_f);
    rotate_head_angle(FRONT, 180, RIGHT);
    while (motor_f.l_enable)
        delay(11);
    
    delay(100);
    
    head_reset(&motor_f);
    rotate_head_angle(FRONT, 90, LEFT);
    while (motor_f.l_enable)
        delay(11);
    digitalWrite(5,LOW);
}

void front_back_head_scan() {
    digitalWrite(5, HIGH);
    
    rotate_head_angle(90, LEFT);
    while (motor_f.l_enable)
        delay(11);
    head_reset(&motor_f);
    head_reset(&motor_b);
    delay(100);
    rotate_head_angle(90, RIGHT);
    while (motor_f.l_enable)
        delay(11);

    head_reset(&motor_f);
    head_reset(&motor_b);
    delay(50);
    rotate_head_angle(90, RIGHT);
    while (motor_f.l_enable)
        delay(11);
    head_reset(&motor_f);
    head_reset(&motor_b);
    delay(100);
    head_reset(&motor_f);
    head_reset(&motor_b);
    rotate_head_angle(90, LEFT);
    while (motor_f.l_enable)
        delay(11);
    
    head_reset(&motor_f);
    head_reset(&motor_b);
    motor_enable(&motor_f,&motor_b, false, false);
    digitalWrite(5, LOW);
    
}

/**
 *
 * @info v pripade ze sa otaca, a narazi na okraj schoda zanou castou, zaisti to aby nepadol
 * @return void
 */
void prekazka_vzadu() {
    motor_off();
    
    uint16_t l_kroky = motor_l.l_kroky;
    uint16_t l_f_kroky = motor_l.l_full_step;
    uint16_t kroky = motor_l.number_step;
    uint16_t f_kroky = motor_l.full_step;
    
    bool l_dir = motor_l.l_dir;
    bool r_dir = motor_r.l_dir;
    
    front_cm(3);
    
    while (motor_l.l_enable)
        delay(11);
    
    motor_l.l_kroky = motor_r.l_kroky = l_kroky;
    motor_l.l_full_step = motor_r.l_full_step = l_f_kroky;
    
    motor_l.number_step = motor_r.number_step = kroky;
    motor_l.full_step = motor_r.full_step = f_kroky;
    
    motor_l.l_dir = l_dir;
    motor_r.l_dir = r_dir;
    
    motor_l.l_enable = true;
    motor_r.l_enable = true;
    motor_enable(&motor_l, &motor_r, true, true);
}
 
/**
 *
 * @info prezentacny algoritmus pre chodenie po stole
 * @param smer - smer kde je stena
 * @return void
 */
void prezentacia_stol() {
    uint8_t stav = 1;
    bool b_stav = true;
    uint16_t x,y=0;
    uint8_t k_vycuvania = 13.5; // konstanta pre vycuvanie aby sa neotocil hned a nepadol

    //rotate_head_angle(BACK, 90, !smer);
    //while (motor_l.l_enable)
    //    delay(11);
    
    do {
        motor_on_front();

        while ((!schody())&&(!proximity_critical_status(&proximity)));
            delay(11);
        
        motor_off();
        
        if (stav == 1) {   
            front_back_head_scan();
            
            stav = 2;
            back_cm(k_vycuvania);
            
            otocenie_robota_k(180,LEFT);

            head_reset_step(&motor_l,&motor_r);
        }else if (stav == 2) {
            front_back_head_scan();
            
            stav = 3;
            
            x = motor_distance(&motor_l) + k_vycuvania;
            Serial.print("Sirka os x: ");
            Serial.println(x);
            
            back_cm(x/2);
            
            otocenie_robota_k(90,LEFT);
            while(motor_l.l_enable)
                delay(11);
            head_reset_step(&motor_l,&motor_r);
            
        } else if (stav == 3) {
            front_back_head_scan();
            
            stav = 4;
            back_cm(k_vycuvania);
            otocenie_robota_k(180,LEFT);
            while(motor_l.l_enable)
                delay(11);
            
            head_reset_step(&motor_l,&motor_r);
        } else if(stav == 4) {
            front_back_head_scan();
            
            y = motor_distance(&motor_l) + k_vycuvania;
            Serial.print("Sirka os y: ");
            Serial.println(y);
            
            back_cm(y/2);
            otocenie_robota(360, LEFT);
            rotate_head_angle(FRONT, 90, LEFT);
            while (motor_f.l_enable)
                delay(11);
            head_reset(&motor_f);
            rotate_head_angle(FRONT,90, RIGHT);
            
            while (motor_l.l_enable)
                delay(11);
            
            
            
            head_reset_step(&motor_l, &motor_r);
            
            b_stav = false;
        }
        
    } while (b_stav);
}

/**
 *
 * @info prezentacny algoritmus pre chodenie po stole bez prednych proximitnych
 * @param smer - smer kde je stena
 * @return void
 */
void prezentacia_stol_e() {
    uint8_t stav = 1;
    bool b_stav = true;
    uint16_t x,y=0;
    uint8_t k_vycuvania = 13.5; // konstanta pre vycuvanie aby sa neotocil hned a nepadol

    //rotate_head_angle(BACK, 90, !smer);
    //while (motor_l.l_enable)
    //    delay(11);
    
    do {
        motor_on_front();

        while (!schody())
            delay(11);
        
        motor_off();
        
        if (stav == 1) {   
            front_back_head_scan();
            
            stav = 2;
            back_cm(k_vycuvania);
            
            otocenie_robota_k(180,LEFT);

            head_reset_step(&motor_l,&motor_r);
        }else if (stav == 2) {
            front_back_head_scan();
            
            stav = 3;
            
            x = motor_distance(&motor_l) + k_vycuvania;
            Serial.print("Sirka os x: ");
            Serial.println(x);
            
            back_cm(x/2);
            
            otocenie_robota_k(90,LEFT);
            while(motor_l.l_enable)
                delay(11);
            head_reset_step(&motor_l,&motor_r);
            
        } else if (stav == 3) {
            front_back_head_scan();
            
            stav = 4;
            back_cm(k_vycuvania);
            otocenie_robota_k(180,LEFT);
            while(motor_l.l_enable)
                delay(11);
            
            head_reset_step(&motor_l,&motor_r);
        } else if(stav == 4) {
            front_back_head_scan();
            
            y = motor_distance(&motor_l) + k_vycuvania;
            Serial.print("Sirka os y: ");
            Serial.println(y);
            
            back_cm(y/2);
            otocenie_robota(360, LEFT);
            rotate_head_angle(FRONT, 90, LEFT);
            while (motor_f.l_enable)
                delay(11);
            head_reset(&motor_f);
            rotate_head_angle(FRONT,90, RIGHT);
            
            while (motor_l.l_enable)
                delay(11);
            
            
            
            head_reset_step(&motor_l, &motor_r);
            
            b_stav = false;
        }
        
    } while (b_stav);
}