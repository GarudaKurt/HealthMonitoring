#ifndef __DISPLAY__H
#define __DISPLAY__H

void initLCD();
void displayWarnHR(const char * message);
void displayData(float temp, uint8_t spo2, uint8_t ecg, unit8_t resplvl);
void displayHeartRateGraph(float heartRate);

#endif
