#ifndef __DISPLAY__H
#define __DISPLAY__H

void initLCD();
void displayWarnHR(const char * message);
void displayData(float temp, float spo2, float ecg, float resplvl);
void displayHeartRateGraph(float heartRate);

#endif
