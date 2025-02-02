#include "ecgSensor.h"
#include <Arduino.h>

#define ecgPin A0  // ECG signal from AD8232 connected to analog pin A0

// Heart rate variables
unsigned long lastBeatTime = 0;   
const int threshold = 512;         
bool pulseAboveThreshold = false; // Ensure this is defined

// Respiratory level variables
const int bufferSize = 200;
int ecgBuffer[bufferSize];
int bufferIndex = 0;
bool bufferFilled = false;  // To check if we have enough data

// Sampling rate (200 Hz)
const unsigned long sampleInterval = 5; 
unsigned long lastSampleTime = 0;

// Store last valid heart rate
static int32_t lastValidBpm = 60;  // Default to a reasonable value

void initECG() {
  pinMode(ecgPin, INPUT);
}

int32_t readECGHr() {
  int ecgSignal = analogRead(ecgPin); 
  static unsigned long timeBetweenBeats = 0;
  static int32_t bpm = lastValidBpm;  

  if (ecgSignal > threshold && !pulseAboveThreshold) { 
    pulseAboveThreshold = true;

    unsigned long currentTime = millis();
    timeBetweenBeats = currentTime - lastBeatTime;
    lastBeatTime = currentTime;

    if (timeBetweenBeats > 300 && timeBetweenBeats < 2000) {  // Avoid unrealistic values
      bpm = 60000 / timeBetweenBeats;
      lastValidBpm = bpm;  // Store last valid value
    }
  }

  if (ecgSignal < threshold) { 
    pulseAboveThreshold = false;
  }

  return lastValidBpm; // Return last known BPM if no new beat is detected
}

int32_t calculateRespiratoryRate() {
  if (!bufferFilled) {
    return 12; // Default to normal breathing rate if buffer isn't full
  }

  long sum = 0;
  long sumSquared = 0;

  for (int i = 0; i < bufferSize; i++) {
    sum += ecgBuffer[i];
    sumSquared += (long)ecgBuffer[i] * ecgBuffer[i];
  }

  float mean = (float)sum / bufferSize;
  float variance = ((float)sumSquared / bufferSize) - (mean * mean);
  int32_t respiratoryLevel = (int32_t)sqrt(variance); 
  return respiratoryLevel;
}

// Function to update buffer
void updateECGBuffer() {
  if (millis() - lastSampleTime >= sampleInterval) {
    ecgBuffer[bufferIndex] = analogRead(ecgPin); // Store the ECG reading
    bufferIndex++;

    if (bufferIndex >= bufferSize) {
      bufferIndex = 0;
      bufferFilled = true;  // Buffer has enough data
    }
    
    lastSampleTime = millis();
  }
}
