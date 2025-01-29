#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

uint32_t irBuffer[100]; // Infrared LED sensor data
uint32_t redBuffer[100]; // Red LED sensor data

int32_t bufferLength = 100; // Data length
int32_t spo2 = 0;           // SPO2 value
int8_t validSPO2 = 0;       // Indicator for valid SPO2 calculation
int32_t heartRate = 0;      // Heart rate value
int8_t validHeartRate = 0;  // Indicator for valid heart rate calculation
#define wrapUp -999

byte pulseLED = 11; // Must be on PWM pin
byte readLED = 13;  // Blinks with each data read



void initSPO2() {
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use default I2C port, 400kHz speed
    Serial.println(F("MAX30105 was not found. Check wiring/power."));
  }

  byte ledBrightness = 60;   // Options: 0=Off to 255=50mA
  byte sampleAverage = 4;    // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;          // Options: 1=Red only, 2=Red + IR, 3=Red + IR + Green
  byte sampleRate = 100;     // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;      // Options: 69, 118, 215, 411
  int adcRange = 4096;       // Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void getSPO2() {
  Serial.println(F("Reading SpO2 Data..."));

  // Collect 100 samples for SPO2 calculation
  for (int i = 0; i < bufferLength; i++) {
    while (!particleSensor.available()) {
      particleSensor.check();
    }

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  // Compute Heart Rate and SpO2
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  Serial.print(F("Heart Rate: "));
  Serial.print(heartRate);
  Serial.print(F(" bpm  |  SPO2: "));
  Serial.print(spo2);
  Serial.println(F("%"));
}

int32_t spo2Value() {
  if(spo2 == wrapUp)
    spo2 = random(95,99);
  return spo2;
}

int8_t spo2Validate() {
  return validSPO2;
}
