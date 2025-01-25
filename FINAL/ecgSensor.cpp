#include "ecgSensor.h"
#include <Arduino.h>

// Define constants without trailing semicolons
#define LO_PLUS 10  // Leads off detection LO+
#define LO_MINUS 11 // Leads off detection LO-
#define ECG_PIN A0  // ECG sensor pin

// Define thresholds for children's heart rate (normal range: 70-120 bpm)
#define MIN_HR 70
#define MAX_HR 120

unsigned long prevMillis = 0;
const int interval = 10000;
int32_t hr = 0;
int32_t peakCount = 0;

const int RESP_INTERVAL = 30000;
unsigned long respPrevMillis = 0;
int32_t baseline = 512;
int32_t respPeakCount = 0;

void initECG() {
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);
}

int32_t readECGHr() {
  int ecgSignal = analogRead(ECG_PIN);

  if (ecgSignal > 512) {
    if (millis() - prevMillis > 300) {
      peakCount++;
    }
  }

  hr = (peakCount * 8);
  peakCount = 0;

  if (hr < MIN_HR || hr > MAX_HR)
    Serial.println("Warning: Heart Rate is not normal!");

  return hr;
}

int32_t calculateRespiratoryRate() {
  static int lastEcgSignal = 0;
  static bool increasing = false;

  int ecgSignal = analogRead(ECG_PIN);

  if (ecgSignal > baseline + 50 && !increasing) {
    increasing = true;
    respPeakCount++;
  }
  if (ecgSignal < baseline - 50)
    increasing = false;

  if (millis() - respPrevMillis >= RESP_INTERVAL) {
    int32_t respiratoryRate = (respPeakCount * 2);
    respPrevMillis = millis();
    respPeakCount = 0;
    return respiratoryRate;
  }
  return 0;
}
