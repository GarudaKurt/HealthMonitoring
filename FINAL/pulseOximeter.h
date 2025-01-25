#ifndef __PULSEOXIMETER__H
#define __PULSEOXIMETER__H

#include <Wire.h>
#include <Arduino.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"


struct SPO2data {
  int32_t data;      // SPO2 value
  int8_t isValid;    // Indicator of validity
};

void initSPO2();
void getSPO2();
int32_t spo2Value();
int8_t spo2Validate();

#endif
