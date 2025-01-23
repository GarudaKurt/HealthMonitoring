#include "Display.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


int graphData[16] = {0}; 
byte bar[8][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Level 0
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F}, // Level 1
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F}, // Level 2
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F}, // Level 3
  {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F}, // Level 4
  {0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}, // Level 5
  {0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}, // Level 6
  {0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}  // Level 7
};

void initLCD(){
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < 8; i++)
    lcd.createChar(i, bar[i]);
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
  delay(2000);
  
}

void displayWarnHR(const char * message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("HR Failure!!!");
}

void displayHeartRateGraph(float heartRate) {
  lcd.clear();
  int graphLevel;

  if (heartRate >= 80 && heartRate <= 100) {
    graphLevel = 7;
  } else if (heartRate >= 70 && heartRate <= 79) {
    graphLevel = 6;
  } else if (heartRate >= 60 && heartRate <= 69) {
    graphLevel = 5;
  } else if (heartRate >= 50 && heartRate <= 59) {
    graphLevel = 4;
  } else {
    graphLevel = random(1, 4);
  }

  for (int i = 0; i < 15; i++) {
    graphData[i] = graphData[i + 1];
  }
  graphData[15] = graphLevel;

  lcd.setCursor(0, 1);
  lcd.print("                ");

  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 1);
    lcd.write(graphData[col]);
  }
}

void displayData(float temp, uint8_t spO2, uint8_t heartRate, uint8_t resplvl) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp, 1);
  lcd.write(byte(223));
  lcd.print("C");

  lcd.setCursor(6, 0);
  lcd.print("|");
  lcd.print(spO2, 0);
  lcd.print("%");

  lcd.setCursor(10, 0);
  lcd.print("|");
  lcd.print(heartRate, 0);

  lcd.setCursor(13, 0);
  lcd.print("|");
  lcd.print(resplvl, 0);
}



