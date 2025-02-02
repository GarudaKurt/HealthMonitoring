#ifndef __ECGSENSOR__H
#define __ECGSENSOR__H

#include <Arduino.h>
void initECG();
int32_t readECGHr();
int32_t calculateRespiratoryRate();
void updateECGBuffer();

#endif
