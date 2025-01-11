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
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
}

void displayWarnHR(const char * message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("HR Failure!!!");
}

void displayData(float temp, float spO2, float heartRate) {
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
  lcd.print("|HR:");
  lcd.print(heartRate, 0);
}

void displayHeartRateGraph(float heartRate) {
  // Map heart rate to graph levels (0 to 7)
  int graphLevel = map(heartRate, 60, 100, 0, 7);
  graphLevel = constrain(graphLevel, 0, 7);

  // Shift graph data to the left
  for (int i = 0; i < 15; i++) {
    graphData[i] = graphData[i + 1];
  }
  graphData[15] = graphLevel;

  // Display the graph
  lcd.setCursor(0, 0);
  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 1);
    lcd.write(graphData[col]);
  }
}

