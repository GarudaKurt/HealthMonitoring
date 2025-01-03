#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ecgPin = A0;      // Pin connected to the AD8232 OUTPUT
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

void setup() {
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, bar[i]);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Real-Time Graph");
}

void loop() {
  int ecgValue = analogRead(ecgPin);

  // Map ECG value to graph levels (0 to 7)
  int graphLevel = map(ecgValue, 512, 1023, 0, 7);
  graphLevel = constrain(graphLevel, 0, 7);
  for (int i = 0; i < 15; i++) {
    graphData[i] = graphData[i + 1];
  }
  graphData[15] = graphLevel;

  // Display graph on LCD
  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 1); // Bottom row
    lcd.write(graphData[col]); // Write graph level
  }

  delay(100);
}
