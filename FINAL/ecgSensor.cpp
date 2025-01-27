#include "ecgSensor.h"
#include <Arduino.h>

#define ecgPin A0  // ECG signal from AD8232 connected to analog pin A0

// Heart rate variables
unsigned long lastBeatTime = 0;    // Time of the last beat
const int threshold = 512;         // Arbitrary threshold for pulse detection
bool pulseAboveThreshold = false;

// Respiratory level variables
const int bufferSize = 200;        // Number of samples for respiratory calculation
int ecgBuffer[bufferSize];
int bufferIndex = 0;

// Sampling rate (200 Hz)
const unsigned long sampleInterval = 5; // Milliseconds between samples
unsigned long lastSampleTime = 0;
void initECG() {
  pinMode(ecgPin, INPUT);
}

int32_t readECGHr() {
  int ecgSignal = analogRead(ecgPin);
  static unsigned long timeBetweenBeats = 0; // Time difference between beats
  static int32_t bpm = 0;

  if (ecgValue > threshold && !pulseAboveThreshold) {
    pulseAboveThreshold = true;

    unsigned long currentTime = millis();
    timeBetweenBeats = currentTime - lastBeatTime;
    lastBeatTime = currentTime;

    if (timeBetweenBeats > 0) {
      bpm = 60000 / timeBetweenBeats; // Calculate BPM
    }
  }

  if (ecgValue < threshold) {
    pulseAboveThreshold = false;
  }

  return bpm;
}

int32_t calculateRespiratoryRate() {
  long sum = 0;
  long sumSquared = 0;

  for (int i = 0; i < bufferSize; i++) {
    sum += ecgBuffer[i];
    sumSquared += (long)ecgBuffer[i] * ecgBuffer[i];
  }

  float mean = (float)sum / bufferSize;
  float variance = ((float)sumSquared / bufferSize) - (mean * mean);
  int32_t respiratoryLevel = (int32_t)sqrt(variance); // Standard deviation as int32_t
  return respiratoryLevel;
}
