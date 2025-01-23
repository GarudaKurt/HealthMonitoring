#include "pulseOximeter.h"

#include <Wire.h>
#include <Arduino.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105* particleSensor = nullptr;

void initSPO2() {
    particleSensor = new MAX30105();

    if (!particleSensor->begin(Wire, I2C_SPEED_STANDARD)) {
        Serial.println("MAX30102 not found. Check connections.");
        while (1);
    }

    particleSensor->setup();  
    particleSensor->setPulseAmplitudeRed(0x0A);
    particleSensor->setPulseAmplitudeIR(0x0A);

    Serial.println("MAX30102 initialized.");
}

uint8_t calculateSPO2(int32_t red, int32_t ir) {
    if (ir == 0) return 0; 

    int32_t ratio = (red * 100) / ir;
    int32_t spo2 = 11000 - (25 * ratio);

    if (spo2 > 10000) spo2 = 10000;
    if (spo2 < 0) spo2 = 0;

    return (uint8_t)(spo2 / 100);
}

uint8_t readSPO2() {
    if (!particleSensor) {
        Serial.println("Sensor not initialized.");
        return 0;
    }

    int32_t red = particleSensor->getRed();
    int32_t ir = particleSensor->getIR();

    // Validate sensor data
    if (red < 500 || ir < 500) {
        Serial.println("No finger detected.");
        delay(1000);
        return 0; // Return 0 if no valid reading
    }

    Serial.print("RED: ");
    Serial.print(red);
    Serial.print(" IR: ");
    Serial.println(ir);

    uint8_t spo2 = calculateSPO2(red, ir);

    Serial.print("Estimated SpO2: ");
    Serial.print(spo2);
    Serial.println(" %");

    return spo2;
}

void cleanupSPO2() {
    if (particleSensor) {
        delete particleSensor; // Free dynamically allocated memory
        particleSensor = nullptr;
        Serial.println("MAX30102 sensor memory deallocated.");
    }
}
