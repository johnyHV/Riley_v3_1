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
 * @param motor_l - pripojenie laveho motora k MCU
 * @param motor_r - pripojenie praveho motora k MCU
 * @param motor_wheel wheel - parametre kolesa a krokoveho motora
 * @param ultrasonic uz_f - pripojenie predneho UZ senzora k MCU
 * @param ultrasonic uz_b - pripojenie zadneho UZ senzora k MCU
 * @param uint8_t sharp_f - pripojenie predneho SHARP senzora k MCU
 * @param uint8_t sharp_b - pripojenie zaneho SHAPR senzora k MCU
 * @param IR_proximity - pripojenie proximitno reflexnych senzorov k MCU
 * @param bool smer - smer jazdy 
 * @return float
 */
float senzor_check(motor_control motor_l, motor_control motor_r, motor_wheel wheel,  uint8_t smer) {
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
 * @param motor_l - pripojenie laveho motora k MCU
 * @param motor_r - pripojenie praveho motora k MCU
 * @param motor_wheel wheel - parametre kolesa a krokoveho motora
 * @param float uhol - uhol o aky sa ma robot otocit
 * @param int smer - smer otocenia
 * @return float
 */
void otocenie_robota(motor_control motor_l, motor_control motor_r, motor_wheel wheel, float uhol, uint8_t smer) {
    
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

void prekazka(motor_control motor_l, motor_control motor_r, motor_control motor_f, motor_control motor_b, motor_wheel wheel) {
    
    head_reset_step(motor_f,motor_b);
    otocenie_robota(motor_l,motor_r,wheel,90,RIGHT);
    
    motor_f.number_step = rotate_head_angle(motor_f,wheel,90,LEFT);
    motor_b.number_step = rotate_head_angle(motor_b,wheel,90,RIGHT);
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
        uint8_t bez_vzdial = 4;
        
            Serial.print("Front: "); Serial.println(front);
            Serial.print("Back: "); Serial.println(back);
        
        bool cykli = true;
        do {
            Serial.print("Kriticke body: "); Serial.println(proximity_critical_status(&proximity));       
            if (!proximity_critical_status(&proximity)) {                         // zisti status kritickych bodov
                Serial.println("Kriticke body OK");
                if((head_distance(uz_f,sharp_f) < front+4)&&(head_distance(uz_f,sharp_f) > front-4)) {                   // zisti ci uz skoncila prekazka pred prednou snimacou hlavou, ak nie ide dalej
                    int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i=0;i<kroky;i++) {
                        motor_step(&motor_l,&motor_r,&wheel, true, false);
                        delayMicroseconds(1750);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                }
                else if((head_distance(uz_b,sharp_b) < back+4)&&(head_distance(uz_b,sharp_b) > back-4)) {                  // ak uz prekazka nie je pred prednou snimacou hlavou, vyjde 5cm
                    int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                    motor_enable(&motor_l,&motor_r,true,true);
                    for (uint16_t i=0;i<kroky;i++) {
                        motor_step(&motor_l,&motor_r,&wheel, true, false);
                        delayMicroseconds(1750);
                    }
                    motor_enable(&motor_l,&motor_r,false,false);
                }
                else {                                                          // ak uz je aj zadna mimo prekazku, vyjde z slucky
                    if (bez_vzdial > 1 ) {
                        int kroky =  ((5.0 / (2.0 *3.14*wheel.r)) * 2048.0);
                        motor_enable(&motor_l,&motor_r,true,true);
                        for (uint16_t i=0;i<kroky;i++) {
                            motor_step(&motor_l,&motor_r,&wheel, true, false);
                            delayMicroseconds(1750);
                        }
                        motor_enable(&motor_l,&motor_r,false,false);
                        bez_vzdial--;
                    }
                    else
                        cykli = false;
                }
            }
            else {
                cykli = false;
            }    
        } while(cykli);
        
    }
     otocenie_robota(motor_l,motor_r,wheel,90,LEFT);
        head_center(motor_f,wheel,RIGHT);
        head_center(motor_b,wheel,LEFT);       
              
  
    /*
    int dlzka = 35;
    
    if (head_distance(uz_f,sharp_f) > 35) {                         // zisti ci pred nim je viac miesta ako 35cm, co je dlzka robota
    while(){
        
    }
        
    if (head_distance(uz_f,sharp_f) > dlzka){
        
        
        
        motor_enable(motor_l,motor_r,true,true);
        
        
        
        
                                         // 0.17578125 = 360/2048
        
        
        
        
        motor_step(motor_l,motor_r,wheel,true,false);
         */
    
        
    
}

void vzdialenost(motor_control motor_l, motor_control motor_r, motor_wheel wheel,motor_control motor_f, motor_control motor_b, uint8_t smer) {
    //uint8_t front = head_distance(uz_f,sharp_f);
    uint8_t back = head_distance(uz_b,sharp_b);
    
    
    if (back < 28) {
        Serial.println("Vzdialenost do 13cm");
        float odstup = 28 - back;
        float obvod = 2.0*3.14*wheel.r;
        float uhol = RadiansToDegrees(atan2(odstup,obvod));
        Serial.print("Uhol pre otocenie: "); Serial.println(uhol);
        
        otocenie_robota(motor_l,motor_r,wheel, uhol,smer);
        motor_enable(&motor_l,&motor_r,true,true);
        for (uint16_t i = 0;i<=2048;i++){
            motor_step(&motor_l,&motor_r,&wheel,true,false);
            delayMicroseconds(1800);
        }
        motor_enable(&motor_l,&motor_r,false,false);
        if (smer == LEFT)
            otocenie_robota(motor_l,motor_r,wheel, uhol,RIGHT);
        else if (smer == RIGHT)
            otocenie_robota(motor_l,motor_r,wheel, uhol,LEFT);
    }
    else if (back > 30) {
        Serial.println("Vzdialenost nad 15cm");
    }
    
}