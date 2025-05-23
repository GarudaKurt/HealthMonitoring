#include "ecgSensor.h"
#include <Arduino.h>

const int LO_PLUS = 10;  // Leads off detection LO+
const int LO_MINUS = 11; // Leads off detection LO-
const int ECG_PIN = A0;

// Define thresholds for children's heart rate (normal range: 70-120 bpm)
const int MIN_HR = 70;
const int MAX_HR = 120;

unsigned long prevMillis = 0;
const int interval = 10000;
int *heartRate = nullptr;
int *peakCount = nullptr;

const int RESP_INTERVAL = 30000; // 30 seconds for respiratory rate calculation
unsigned long respPrevMillis = 0;
int baseline = 512; // Approximate midpoint for the ECG signal
int *respPeakCount = nullptr;

void initECG() {
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);

  // Dynamically allocate memory for variables
  heartRate = new int(0);
  peakCount = new int(0);
  respPeakCount = new int(0);
}

uint8_t readECGHr() {
    int ecgSignal = analogRead(ECG_PIN);

    // Check for peaks (simple threshold for peak detection)
    if (ecgSignal > 512) {  // Adjust threshold as per your ECG signal
        if (millis() - prevMillis > 300) {  // Debounce to avoid false peaks
            prevMillis = millis();
            (*peakCount)++;
        }
    }

    *heartRate = (*peakCount * 10); // Convert 10-second peaks to bpm
    *peakCount = 0;

    Serial.print("Heart Rate: ");
    Serial.print(*heartRate);
    Serial.println(" bpm");

    // Check if heart rate is normal
    if (*heartRate < MIN_HR || *heartRate > MAX_HR)
        Serial.println("Warning: Heart Rate is not normal!");
    else
        Serial.println("Heart Rate is normal.");

    return *heartRate;
}

uint8_t calculateRespiratoryRate() {
  static int lastEcgSignal = 0;
  static bool increasing = false;

  int ecgSignal = analogRead(ECG_PIN);

  // Check for respiratory signal peaks
  if (ecgSignal > baseline + 50 && !increasing) {
    increasing = true;
    (*respPeakCount)++;
  }
  if (ecgSignal < baseline - 50)
    increasing = false;

  // Every RESP_INTERVAL, calculate and reset the respiratory rate
  if (millis() - respPrevMillis >= RESP_INTERVAL) {
    uint32_t respiratoryRate = (*respPeakCount * 2); // Convert 30-second peaks to breaths per minute
    respPrevMillis = millis();
    *respPeakCount = 0;

    Serial.print("Respiratory Rate: ");
    Serial.print(respiratoryRate);
    Serial.println(" breaths per minute");

    return respiratoryRate;
  }
  Serial.println("Test not passed!");
  return 0; 
}

void cleanupECG() {
  // Free allocated memory
  if (heartRate != nullptr) {
    delete heartRate;
    heartRate = nullptr;
  }

  if (peakCount != nullptr) {
    delete peakCount;
    peakCount = nullptr;
  }

  if (respPeakCount != nullptr) {
    delete respPeakCount;
    respPeakCount = nullptr;
  }
}
