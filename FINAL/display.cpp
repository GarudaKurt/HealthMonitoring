#include "display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

byte graphData[16] = {0}; // Use byte instead of int for memory efficiency
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

void initLCD() {
  lcd.init();
  lcd.backlight();
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, bar[i]);
  }
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");
  delay(2000);
}

void displayWarnHR(const char* message, const char * message1) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.print(" ");
  lcd.print(message1);
  lcd.setCursor(1,1);
  lcd.print("Action Required!");
}

void clearDisplay() {
  lcd.clear();
}

void updateDisplay(double temp, int32_t spO2, int32_t heartRate, int32_t resplvl) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp, 1);  // Temperature
  lcd.write(byte(223));  // Degree symbol
  lcd.print("C|");

  lcd.print(spO2);  // SpO2
  lcd.print("%|");

  lcd.print(heartRate);  // Heart rate
  lcd.print("|");

  lcd.print(resplvl);
}


void displayGraph(int32_t heartRate) {
  int32_t graphLevel;
  if (heartRate >= 80 && heartRate <= 100) {
    graphLevel = 7;
  } else if (heartRate >= 70 && heartRate <= 79) {
    graphLevel = 6;
  } else if (heartRate >= 60 && heartRate <= 69) {
    graphLevel = 5;
  } else if (heartRate >= 50 && heartRate <= 59) {
    graphLevel = 4;
  } else {
    graphLevel = 3;  // Avoid random for readability
  }

  for (int i = 0; i < 15; i++) {
    graphData[i] = graphData[i + 1];
  }
  graphData[15] = graphLevel;

  lcd.setCursor(0, 1);
  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 1);
    lcd.write(graphData[col]);
  }
}
