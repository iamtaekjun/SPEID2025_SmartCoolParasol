#include <Arduino.h>

#define RELAY_PIN 6
#define STATUS_LED 13

bool pumpRunning = false;
unsigned long pumpStartTime = 0;

void pumpON() {
    Serial.println("Pump ON");
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(STATUS_LED, HIGH);
    pumpRunning = true;
    pumpStartTime = millis();
}

void pumpOFF() {
    Serial.println("Pump OFF");
    digitalWrite(RELAY