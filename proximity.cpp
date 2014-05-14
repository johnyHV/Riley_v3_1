#include "proximity.h"

/**
 *
 * @info Inicializacia proximitno reflexnych IR senzorov. Prvotna inicializacia vybera 0 senzor
 * @param proximity - struktura pripojenia senzorov k MCU
 * @param mux - objekt na multiplexor
 * @return void
 */
void proximity_initial(IR_proximity *proximity, mux4067 &mux) {
    for (uint8_t i = 0; i < 3; i++) {
        pinMode(proximity->critical[i], INPUT);
        pinMode(proximity->comparator[i], INPUT);
    }
    mux.initial(proximity->mux); // prepnutie multiplexorov pre IR takticke senzory
    mux.pin_select(0); // vybratie 0 senzora pri prvotnej inicializacii
}

/**
 *
 * @info Vycitanie hodnot z kritickych bodov
 * @param proximity - struktura pripojenia senzorov k MCU
 * @return Stav kritickych senzorov true-prekazka, false-volno
 */
bool proximity_critical_status(IR_proximity *proximity) {
    if ((digitalRead(proximity->critical[0])) || (digitalRead(proximity->critical[1])))
        return true;
    else
        return false;
}

/**
 *
 * @info Digitalny status vybraneho proximitno reflexneho IR senzora
 * @param proximity - struktura pripojenia senzorov k MCU
 * @param sensor - cislo senzoru
 * @param mux - objekt na multiplexor
 * @param number_comparator - cislo komparatora (1,2), 3 automaticka kontrola funmcnosti komparatorov
 * @return logicka hodnota komparatora
 */
uint8_t proximity_d_status(IR_proximity *proximity, uint8_t sensor, mux4067 &mux, uint8_t number_comparator) {
    mux.initial(proximity->mux); // prepnutie multiplexorov pre IR takticke senzory
    mux.pin_select(sensor);

    if (number_comparator == 3) {
        bool comp_1 = digitalRead(proximity->comparator[0]);
        bool comp_2 = digitalRead(proximity->comparator[1]);

        if (comp_1 == comp_2)
            return comp_1;
        else
            return 3;
    } else {
        return digitalRead(proximity->comparator[number_comparator]);
    }
}

/**
 *
 * @info Analogovy status vybraneho proximitno reflexneho IR senzora
 * @param proximity - struktura pripojenia senzorov k MCU
 * @param sensor - cislo senzoru
 * @param mux - objekt na multiplexor
 * @param number_comparator - cislo komparatora
 * @return analogova hodnota z demodulatora
 */
float proximity_a_status(IR_proximity *proximity, uint8_t sensor, mux4067 &mux) {
    mux.initial(proximity->mux); // prepnutie multiplexorov pre IR takticke senzory
    mux.pin_select(sensor);
    //delayMicroseconds(3000);
    //mux.pin_select(sensor-5);
    //analogReadResolution(10);
    return (analogRead(A3) * (3.3 / 1023.0));
    //return analogRead(proximity->comparator[number_comparator]);
}

/**
 *
 * @info Ziskanie statusu ci sa  pred robotom nenachadzaju schody
 * @return bool - 1 - schody su, 0- schody niesu
 */
bool schody() {
    mux.initial(proximity.mux); // prepnutie multiplexorov pre IR takticke senzory
    mux.pin_select(proximity.schody[0]);

    delay(50);
    bool comp_1_l = digitalRead(proximity.comparator[0]);
    bool comp_2_l = digitalRead(proximity.comparator[1]);

    delay(50);
    mux.pin_select(proximity.schody[1]);
    bool comp_1_r = digitalRead(proximity.comparator[0]);
    bool comp_2_r = digitalRead(proximity.comparator[1]);

    if ((comp_1_l == comp_2_l)&&(comp_1_r == comp_1_l))
        return !comp_1_l;
    else
        return true;
}

/**
 *
 * @info Ziskanie statusu ci sa  za robotom nenachadzaju schody
 * @return bool - 1 - schody su, 0- schody niesu
 */
bool schody_back() {
    mux.initial(proximity.mux); // prepnutie multiplexorov pre IR takticke senzory
    mux.pin_select(15);

    delay(50);
    bool comp_1_l = digitalRead(proximity.comparator[0]);
    bool comp_2_l = digitalRead(proximity.comparator[1]);

    if (comp_1_l == comp_2_l)
        return !comp_1_l;
    else
        return true;
}

/**
 *
 * @info Inicializacia A/D prevodnika pre SHARP proximitno triangulacny senzor
 * @return void
 */
void sharp_init() {
    analogReadResolution(10);
}

/**
 *
 * @info Vypocet vzdialenosti od Proximitno triangulacneho IR Sharp senzora
 * @param Adresa senzora pre A/D prevodnik
 * @return vzdialenost v cm
 */
float sharp_distance(uint8_t addres) {
    analogReadResolution(10);
    float x = (analogRead(addres) * (3.3 / 1024));
    //return voltage;
    //return 10650.08 * pow(analogRead(addres),-0.935) - 10;
    //return (1 / (0.0002391473 * analogRead(addres) - 0.0100251467));
    //return  65*pow(voltage, -1.10);
    //return (1 / (0.0002391473 * voltage - 0.0100251467));
    //return (analogRead(addres) * (3.3 / 4095));
    //
    //return (306.439 + voltage * (-512.611 + voltage * (382.268 + voltage * (-129.893 + voltage * 16.2537))));
    //return (6050/voltage);
    return (16.2537 * pow(x, 4) - 129.893 * pow(x, 3) + 382.268 * pow(x, 2) - 512.611 * x + 306.439);
    //return (306.439 + x * ( -512.611 + x * ( 382.268 + x * (-129.893 + x * 16.2537) )));

}
