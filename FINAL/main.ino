#include <Wire.h>
#include <Arduino.h>

#include "pulseOximeter.h"
#include "temperature.h"
#include "ecgSensor.h"
#include "display.h"

const int interval = 1000;
unsigned long prev = 0;

void setup() {
  Serial.begin(9600);
  initSPO2();
  //initLCD();
  //initTemperature();
  //initECG();
}

void loop() {

  unsigned long current = millis();
  if(current - prev >= interval) {
    getSPO2();

    //double temp = readTemperature();
    int32_t spo2 = spo2Value();
    //int32_t respiratoryRate = calculateRespiratoryRate();
    //int32_t hr = readECGHr();
    
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

    if(temp > 37.50)
      displayWarnHR(temp, "Temp");
    
    //updateDisplay(temp, spo2, hr, respiratoryRate );  
    prev = current;
  }

}
