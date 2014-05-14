#include "calculation.h"


/**
 *
 * @info Vrati prejdenu vzdialenost daneho motora v cm
 * @param motor - pripojenie motora k MCU
 * @return float
 */
float motor_distance(motor_control *motor, motor_wheel * wheel) {
    
    
    /*int cele_otacky = ((int) motor->number_step / wheel->step_360);
    float cela_vzdialenost = 2 * 3.14 * wheel->r * cele_otacky;

    int zvysok_krokov = ((int) motor->number_step % wheel->step_360);
    float zvysna_vzdialenost = (2 * 3.14 * wheel->r * (wheel->step_360 / 360) * zvysok_krokov) / 360;

    return cela_vzdialenost + zvysna_vzdialenost;
     */
    
    float cela_vzdialenost = 2 * 3.14 * wheel->r * motor->full_step;
    
    int zvysok_krokov = ((int) motor->number_step % (int) wheel->step_360);
    float zvysna_vzdialenost = (2 * 3.14 * wheel->r * (wheel->step_360 / 360) * zvysok_krokov) / 360;

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
 * @info Zisti realnu hodnotu vzdialenosti pri nezhode UZ a SHARP senzoru
 * @param bool smer - smer jazdy 
 * @return float
 */
float senzor_check(uint8_t smer) {
    if (smer == FRONT) {
        Serial.println("start kontroly");
        float f_sharp_d = sharp_distance(sharp_f);              // vzdialenost v povodnej polohe z sharp
        float f_uz_d = ultrasonic_distance_(&uz_f);             // vzdialenost v povodnej polohe z UZ
        float back_distance = head_distance(uz_b, sharp_b);     // vzdialenost zadnej snimacej hlavy v povodnej polohe
        uint16_t d_back=0;
        float f_back=0.0;
        
        if (back_distance == 0.0)
            back_distance = ultrasonic_distance_(&uz_b);
        
        Serial.print("f_sharp: ");Serial.println(f_sharp_d);
        Serial.print("f_uz: ");Serial.println(f_uz_d);
        Serial.print("Back distance: ");Serial.println(back_distance);
        
        // pokial je za robotom miesto viac ako 7cm, vycuva 5cm, pokial nie vycuva 80% vzdialenosti
        if (back_distance > 7) {
            d_back = (((360.0*5.0) / (2.0 *3.14*3.3)) / (360.0/2048.0));                                 // 0.17578125 = 360/2048            
            f_back = 5.0;
        }
        else if (back_distance > 1) {
            d_back = ((360.0 * ((back_distance / 100.0)*80.0)) / 2.0 * 3.14 * 3.3) / (360.0/2048.0);      // 0.17578125 = 360/2048
            f_back = (back_distance / 100.0)*80.0;
        }
        Serial.print("d_back: ");Serial.println(d_back);
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
        
        Serial.print("f_sharp: ");Serial.println(n_f_sharp_d);
        Serial.print("f_uz: ");Serial.println(n_f_uz_d);
        Serial.print("Back distance: ");Serial.println(n_back_distance);        

        // vrati sa na podovne miesto
        motor_enable(&motor_l, &motor_r, true, true);
        for (uint16_t n_steps = 0; n_steps <= d_back; n_steps++) {
            motor_step(&motor_l, &motor_r, &wheel, true, false);
            delayMicroseconds(2000);
        }
        motor_enable(&motor_l, &motor_r, false, false);

        // kontrola spravnosti UZ
        float rozdiel_uz = n_f_uz_d - f_uz_d - f_back;
        Serial.print("rozdiel UZ: ");Serial.println(rozdiel_uz);
        if ((rozdiel_uz > -2.5) && (rozdiel_uz < 2.5)){
            UZ = true;
            Serial.println("Spravny UZ");
        }
            

        // kontrola spravnosti IR
        float rozdiel_ir = n_f_sharp_d - f_sharp_d - f_back;
        Serial.print("rozdiel IR: ");Serial.println(rozdiel_ir);
        if ((rozdiel_ir > -2.5) && (rozdiel_ir < 2.5)){
            Serial.println("Spravny IR");
            IR = true;
        }
            

        // kontrola ci realne vycuval cca 5cm
        float rozdiel_back = back_distance - n_back_distance - f_back;
        Serial.print("rozdiel v vycuvani: ");Serial.println(rozdiel_back);
        if ((rozdiel_back > -2.5) && (rozdiel_back < 2.5)){
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

        if (IR){
            Serial.println("IR");
            return f_sharp_d;
        }
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
    
    float obvod =  (3.14 * wheel.d * uhol) / 360.0;
    int kroky = (obvod / (2.0 * 3.14 * wheel.r)) * wheel.step_360;
    motor_enable(&motor_l,&motor_r,true,true);
    
    Serial.println(obvod);
    Serial.println(kroky);
    Serial.println(azimuth_cisty());
    
    if (smer == LEFT){
        for (uint16_t i=0; i <= kroky; i++){
            motor_step(&motor_l,&motor_r,&wheel,false,false);
            delayMicroseconds(2000);
            //if ((kroky % 10) == 0)
                //qSerial.println(azimuth_cisty());
        }
    }
    else if (smer == RIGHT) {
        for (uint16_t i=0; i <= kroky; i++){
            motor_step(&motor_l,&motor_r,&wheel,true,true);
            delayMicroseconds(2000);
        }
    }
    motor_enable(&motor_l,&motor_r, false, false);
      
}

/**
 *
 * @info vyhne sa prekazke
 * @return void
 */
void prekazka() {
    
    head_reset_step(&motor_f,&motor_b);
    otocenie_robota(90,RIGHT);    
    rotate_head_angle(FRONT,90,LEFT);
    motor_f.smer_otocenia = LEFT;
    motor_f.uhol_otocenia = 90;
    rotate_head_angle(BACK,90,RIGHT);
    motor_b.smer_otocenia = RIGHT;
    motor_b.uhol_otocenia = 90;
    
    uint8_t front = head_distance(uz_f,sharp_f);
    uint8_t back = head_distance(uz_b,sharp_b);
    Serial.print("Front: "); Serial.println(front);
    Serial.print("Back: "); Serial.println(back);
    
    //delay(5000);
    //head_center(motor_f,motor_b,wheel,RIGHT);
    
    if (((front - back) < 5) && ((front - back) > -5)){                 // zisti, ci prekazka je sirsia ako on sam. ci predny senzor uz nie je mimo prekazku
        //head_center(motor_f,wheel,RIGHT);
        //head_reset_step(motor_f);
        Serial.println("Hodnoty su rovnake: ");
        uint8_t front = head_distance(uz_f,sharp_f);
        uint8_t back = head_distance(uz_b,sharp_b);
        uint8_t bez_vzdial = 3;
        uint8_t n_back =0;
        uint8_t n_front =0;
        Serial.print("Front: "); Serial.println(front);
        Serial.print("Back: "); Serial.println(back);
        uint8_t stav = 0;
        bool cykli = true;
        do {
            
            Serial.print("Kriticke body: "); Serial.println(proximity_critical_status(&proximity));       
            if (!proximity_critical_status(&proximity)) {                         // zisti status kritickych bodov
                Serial.println("Kriticke body OK");
                uint8_t poistka_f =0;
                uint8_t poistka_b =0;
                
                n_front = head_distance(uz_f,sharp_f);
                n_back = head_distance(uz_b,sharp_b);
                
                while (n_front == 0){                        // kontrola pre pripadnu 0 hodnotu z hlavy. ak je 0 hodnota 10 krat, neskusa znova merat
                    n_front = head_distance(uz_f,sharp_f);
                    poistka_f++;
                    if (poistka_f >5)
                        break;
                    //Serial.print("poistka: "); Serial.println(poistka_f);
                }
                while (back == 0) {
                    back = head_distance(uz_b,sharp_b);
                    poistka_b++;
                    if (poistka_b > 5)
                        break;
                }
                
                Serial.print("Predna: "); Serial.println(n_front);
                Serial.print("Zadna: "); Serial.println(n_back);
                Serial.print("front: "); Serial.println(front);
                Serial.print("back: "); Serial.println(back);
                
                if((n_front < front+10)&&(n_front > front-10)) {                   // zisti ci uz skoncila prekazka pred prednou snimacou hlavou, ak nie ide dalej
                    Serial.println("Predna OK");
                    int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i=0;i<kroky;i++) {
                        motor_step(&motor_l,&motor_r,&wheel, true, false);
                        delayMicroseconds(1750);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                }
                else if((n_back < back+10)&&(n_back > back-10)) {                  // ak uz prekazka nie je pred prednou snimacou hlavou, ide pokial nie je aj pred zadnou
                    Serial.println("Zadna OK");
                    int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i=0;i<kroky;i++) {
                        motor_step(&motor_l,&motor_r,&wheel, true, false);
                        delayMicroseconds(1750);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                }
                else if (bez_vzdial > 1 ) {                                                                                                  // ak uz je aj zadna mimo prekazku, vyjde z slucky
                    Serial.println("Dodatocny posun");
                    int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i=0;i<kroky;i++) {
                        motor_step(&motor_l,&motor_r,&wheel, true, false);
                        delayMicroseconds(1750);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                    bez_vzdial--;
                }
                else {
                    if (stav == 0){
                        bez_vzdial = 3;
                        Serial.println("Zmena stavu na 1 + otocka");
                        otocenie_robota(90,LEFT);
                        front = head_distance(uz_f,sharp_f);
                        back = head_distance(uz_b,sharp_b);
                        stav++;
                    }
                    else
                        cykli = false;
                }    
            }
            else {
                cykli = false;
            }    
        } while(cykli);
        
        if (motor_f.smer_otocenia == LEFT)
            head_center(motor_f,wheel,RIGHT);
        else if (motor_f.smer_otocenia == RIGHT)
            head_center(motor_f,wheel,LEFT);
        
        motor_f.smer_otocenia = CENTER;
        motor_f.uhol_otocenia = 0;
        head_reset_step(&motor_f);
    }
    else {
        otocenie_robota(90,LEFT);
        
        if (motor_f.smer_otocenia == LEFT)
            head_center(motor_f,wheel,RIGHT);
        else if (motor_f.smer_otocenia == RIGHT)
            head_center(motor_f,wheel,LEFT);
        
        motor_f.smer_otocenia = CENTER;
        motor_f.uhol_otocenia = 0;
        head_reset_step(&motor_f);
    }
}

/**
 *
 * @info odide od prekazky o 13cm
 * @param uint8?t smer - smer v ktorom ma ist prec od steny
 * @return void
 */
void vzdialenost_stena(uint8_t smer) {

    if (motor_f.smer_otocenia == CENTER){                                       // pokial je to center, otoci sa v proti smere otacania robota aby sa pozeralo na stenu
        
        uint8_t d_smer;
        if (smer == LEFT)
            d_smer = RIGHT;
        else
            d_smer = LEFT;
        
        rotate_head_angle(FRONT,90,d_smer);
        motor_f.smer_otocenia = d_smer;
        Serial.print("Smer otocenia"); Serial.println(d_smer);
        motor_f.uhol_otocenia = 90;
    }
    else if (motor_f.smer_otocenia != smer) {
        rotate_head_angle(FRONT,90,smer);
        motor_f.smer_otocenia = smer;
        motor_f.uhol_otocenia = 180; 
    }
    /* pre zadnu snimaciu hlavu nie prednu
    if (motor_f.smer_otocenia == CENTER){                                       // pokial je to center, otoci sa v proti smere otacania robota aby sa pozeralo na stenu
        rotate_head_angle(BACK,90,smer);
        motor_f.smer_otocenia = smer;
        motor_f.uhol_otocenia = 90;
    }
    else if (motor_f.smer_otocenia != smer) {
        uint8_t d_smer;
         if (smer == LEFT)
                d_smer = RIGHT;
        else
                d_smer = LEFT;
        rotate_head_angle(BACK,90,d_smer);
        motor_f.smer_otocenia = d_smer;
        motor_f.uhol_otocenia = 180;    
    }
    */
    uint8_t front = head_distance(uz_f,sharp_f);
    
    
    if (front < 28) {
        Serial.println("Vzdialenost do 13cm");
        float odstup = 28 - front;
        float obvod = 2.0*3.14*wheel.r;
        float uhol = RadiansToDegrees(atan2(odstup,obvod));
        
        otocenie_robota(uhol,smer);
        motor_enable(&motor_l,&motor_r,true,true);
        for (uint16_t i = 0;i<=2048;i++){
            motor_step(&motor_l,&motor_r,&wheel,true,false);
            delayMicroseconds(1800);
        }
        motor_enable(&motor_l,&motor_r,false,false);
        if (smer == LEFT) {
            otocenie_robota(uhol,RIGHT);
        }
        else if (smer == RIGHT) {
            otocenie_robota(uhol,LEFT);
        }
        if (motor_f.smer_otocenia == LEFT)
            head_center(motor_f,wheel,RIGHT);
        else if (motor_f.smer_otocenia == RIGHT)
            head_center(motor_f,wheel,LEFT);
        
        motor_f.smer_otocenia = CENTER;
        motor_f.uhol_otocenia = 0;
        head_reset_step(&motor_f);
    }
    else if (front > 28) {
        Serial.println("Vzdialenost nad 15cm");
        float odstup = front - 28;
        float obvod = 2.0*3.14*wheel.r;
        float uhol = RadiansToDegrees(atan2(odstup,obvod));
        
        uint8_t n_smer;
        if (smer == LEFT)
            n_smer = RIGHT;
        else
            n_smer = LEFT;
        
        otocenie_robota(uhol,n_smer);
        motor_enable(&motor_l,&motor_r,true,true);
        for (uint16_t i = 0;i<=2048;i++){
            motor_step(&motor_l,&motor_r,&wheel,true,false);
            delayMicroseconds(1800);
        }
        motor_enable(&motor_l,&motor_r,false,false);
        if (n_smer == LEFT) {
            otocenie_robota(uhol,RIGHT);
        }
        else if (n_smer == RIGHT) {
            otocenie_robota(uhol,LEFT);
        }
        if (motor_f.smer_otocenia == LEFT)
            head_center(motor_f,wheel,RIGHT);
        else if (motor_f.smer_otocenia == RIGHT)
            head_center(motor_f,wheel,LEFT);
        
        motor_f.smer_otocenia = CENTER;
        motor_f.uhol_otocenia = 0;
        head_reset_step(&motor_f);
    }
}