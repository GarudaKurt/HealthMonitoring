#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

// Buffers for IR and Red LED data
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
uint16_t irBuffer[100]; // Infrared LED sensor data
uint16_t redBuffer[100]; // Red LED sensor data
#else
uint32_t irBuffer[100]; 
uint32_t redBuffer[100];
#endif

int32_t spo2; // SpO2 value
int8_t validSPO2; // Indicator for valid SpO2 value
int32_t bufferLength = 100; // Number of samples to store in the buffer

void setup() {
  Serial.begin(115200);

  // Initialize the MAX30105 sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not detected. Please check wiring.");
    while (1); // Stop execution if sensor initialization fails
  }

  // Sensor configuration
  byte ledBrightness = 60; // LED brightness
  byte sampleAverage = 4; // Number of samples to average
  byte ledMode = 2; // Red and IR LEDs enabled
  byte sampleRate = 100; // Sampling rate (in Hz)
  int pulseWidth = 411; // LED pulse width
  int adcRange = 4096; // ADC range

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop() {
  double spO2 = getSpO2(); // Get the SpO2 value
  Serial.print("SpO2: ");
  if (spO2 < 0) {
    Serial.println("Invalid"); // Handle invalid values
  } else {
    Serial.print(spO2, 1);
    Serial.println(" %");
  }

  delay(1000); // Wait for 1 second before the next reading
}

double getSpO2() {
  // Fill the buffer with initial readings
  for (int i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false)
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  // Calculate SpO2 using the algorithm
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, nullptr, nullptr);

  if (validSPO2) {
    return spo2; // Return valid SpO2 value
  } else {
    return -1.0; // Return -1 for invalid values
  }
}
