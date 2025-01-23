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

const int RESP_INTERVAL = 30000;
unsigned long respPrevMillis = 0;
int baseline = 512;
int *respPeakCount = nullptr;

void initECG() {
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);

  heartRate = new int(0);
  peakCount = new int(0);
  respPeakCount = new int(0);
}

uint8_t readECGHr() {
    int ecgSignal = analogRead(ECG_PIN);

    if (ecgSignal > 512) {
        if (millis() - prevMillis > 300) {
            (*peakCount)++;
        }
    }

    *heartRate = (*peakCount * 8);
    *peakCount = 0;

    if (*heartRate < MIN_HR || *heartRate > MAX_HR)
        Serial.println("Warning: Heart Rate is not normal!");

    return *heartRate;
}

uint8_t calculateRespiratoryRate() {
  static int lastEcgSignal = 0;
  static bool increasing = false;

  int ecgSignal = analogRead(ECG_PIN);

  if (ecgSignal > baseline + 50 && !increasing) {
    increasing = true;
    (*respPeakCount)++;
  }
  if (ecgSignal < baseline - 50)
    increasing = false;

  if (millis() - respPrevMillis >= RESP_INTERVAL) {
    uint32_t respiratoryRate = (*respPeakCount * 2); /
    respPrevMillis = millis();
    *respPeakCount = 0;
    return respiratoryRate;
  }
  return 0;

}

void cleanupECG() {
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
