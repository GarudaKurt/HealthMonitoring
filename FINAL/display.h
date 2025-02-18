#ifndef __DISPLAYLCD__H
#define __DISPLAYLCD__H
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>


void initLCD();
void updateDisplay(double temp, int32_t spo2, int32_t heartRate, int32_t resplvl);
void displayWarnHR(const char * message, const char * messaga1);
void clearDisplay();
void displayGraph(int32_t heartRate);


#endif
