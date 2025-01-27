#include <Wire.h>
#include <Arduino.h>

#include "pulseOximeter.h"
#include "temperature.h"
#include "ecgSensor.h"
#include "display.h"

const int interval1 = 1000;
const int interval2 = 5000;
unsigned long prev1 = 0;
unsigned long prev2 = 0;
int32_t hr = 0;

void setup() {
  Serial.begin(9600);
  initSPO2();
  //initLCD();
  //initTemperature();
  //initECG();
}

void loop() {

  unsigned long current = millis();

  if(current - prev1 >= interval1) {
    hr = readECGHr();
    displayGraph(hr);
    prev1 = current;
  }
  
  if(current - prev2 >= interval2) {
    getSPO2();

    //double temp = readTemperature();
    int32_t spo2 = spo2Value();
    //int32_t respiratoryRate = calculateRespiratoryRate();
    //hr = readECGHr();
    
    //Serial.print("Body Temp: ");
    //Serial.printlnI(readTemperature());

    //Serial.print("respiratoryRate: ");
    //Serial.printlnI(calculateRespiratoryRate());

    //Serial.print("Heart Rate: ");
    //Serial.printlnI(readECGHr());
    
    Serial.println(spo2Value());
    Serial.print("Validate: ");
    Serial.println(spo2Validate());
    Serial.print("SPO2: ");

    if(spo2Validate == 0)
      spo2 = 0;

    if(temp > 37.50) {
      displayWarnHR(temp, "Temp");
      delay(5000);
      clearDisplay();
    }
    //updateDisplay(temp, spo2, hr, respiratoryRate );  
    prev2 = current;
  }

}
