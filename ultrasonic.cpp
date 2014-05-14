#include "ultrasonic.h"

/**
 *
 * @info Inicializacia ultrazvukoveho senzora
 * @param ultrasonic - struktura pripojenia senzorov k MCU
 * @return void
 */
void ultrasonic_initial(ultrasonic *data) {
    pinMode(data->echo, INPUT);
    pinMode(data->trig, OUTPUT);
    digitalWrite(data->trig, LOW);
}

/**
 *
 * @info Cista zmerana vzdialenost objektu
 * @param ultrasonic - struktura pripojenia senzorov k MCU
 * @return vzdialenost v cm
 */
float ultrasonic_distance(ultrasonic *data) {
    long t_micro = micros();
    long temp_b = t_micro + 21000;

    digitalWrite(data->trig, LOW);
    delayMicroseconds(2);

    digitalWrite(data->trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(data->trig, LOW);

    long out = pulseIn(data->echo, HIGH, 20000);

    if (micros() <= temp_b)
        out = out / 58;
    else
        out = 400;

    return out;
}

/**
 *
 * @info Zmerana vzdialenost 10x, vybrata najpravdepodobnejsia vzdialenost, spriemerovana
 * @param ultrasonic - struktura pripojenia senzorov k MCU
 * @param debug - 1 - vypis hodnot merania
 * @return vzdialenost v cm
 */
float ultrasonic_distance_(ultrasonic *data, uint8_t debug) {
    // merania
    long out[10] = {0};
    for (uint8_t i = 0; i < 10; i++) {
        digitalWrite(data->trig, LOW);
        delayMicroseconds(2);

        digitalWrite(data->trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(data->trig, LOW);

        out[i] = pulseIn(data->echo, HIGH) / 58;

        delay(20);
    }

    // zistenie ktora hodnota je realna
    uint8_t pocet[10] = {0};
    uint8_t max = 0;
    uint8_t prvok = 0;
    for (uint8_t j = 0; j < 10; j++) {
        for (uint8_t i = 0; i < 10; i++)
            if (out[j] == out[i])
                pocet[j]++;
            else if ((out[i] >= (out[j] - 1)) && (out[i] <= (out[j] + 1)))
                pocet[j]++;

        if (pocet[j] > max) {
            max = pocet[j];
            prvok = j;
        }
    }

    if (debug) {
        Serial.println("Vyskyt merania");
        for (uint8_t j = 0; j < 10; j++)
            Serial.println(pocet[j]);

        Serial.println("Vysledok merani");
        for (uint8_t j = 0; j < 10; j++)
            Serial.println(out[j]);
    }

    return out[prvok];
}


/*
long Ultrasonic::Timing()
{
  digitalWrite(Trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  duration = pulseIn(Echo_pin,HIGH);
  return duration;
}

long Ultrasonic::Ranging(int sys)
{
  Timing();
  distacne_cm = duration /29 / 2 ;
  distance_inc = duration / 74 / 2;
  if (sys)
  return distacne_cm;
  else
  return distance_inc;
} 
 */
