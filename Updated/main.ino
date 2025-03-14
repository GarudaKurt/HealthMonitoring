#include <Arduino.h>
#include "ecgSensor.h"
#include "temperature.h"
#include "Display.h"

unsigned long prevTime = 0;
const long interval = 1000;

unsigned long prevHRTime = 0;
const int intervalHR = 10000;

const int MIN_HR = 70;
const int MAX_HR = 120;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initECG();
  initTemperature();
  initLCD();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  if(currentTime - prevTime >= interval) {
 
    readSPO2();
    readTemperature();
    readECGHr();

    displayData(readSPO2(), readTemperature(), readECGHr(), calculateRespiratoryRate();
    displayHeartRateGraph(readECGHr());
    if(!isSPO2Valid()) {
      displayWarnHR("Warn SPO2!!")
      buzzerStart();
    } else {
      buzzerStop();
    }
    if(readTemperature() > 38) {
      displayWarnHR("Warn TEMP!!");
      buzzerStart();
    } else {
      buzzerStop();
    }

    currentTime = prevTime;
  }

  if(currentTime - prevHRTime >= intervalHR) {
    if(readECGHr() < MIN_HR || readECGHr() > MAX_HR) {
      displayWarnHR("Warn HR!!");
      buzzerStart();
    } else {
      buzzerStop();
    }
    prevHRTime = currentTime;
  }

}
