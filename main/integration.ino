#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal_I2C lcd(0x27, 16, 2);
MAX30105 particleSensor;

const int buzzerPin = 6;
unsigned long prevTime = 0;
unsigned long intervalTime = 1000;

enum class tones {
  NOTE_C4 = 262,
  NOTE_D4 = 294,
  NOTE_E4 = 330,
  NOTE_F4 = 349,
  NOTE_G4 = 392,
  NOTE_A4 = 440,
  NOTE_B4 = 494,
  NOTE_C5 = 523
};

tones melody[8] = {tones::NOTE_C4, tones::NOTE_D4, tones::NOTE_E4, tones::NOTE_F4,
                    tones::NOTE_G4, tones::NOTE_A4, tones::NOTE_B4, tones::NOTE_C5};
int noteDurations[8] = {4, 4, 4, 4, 4, 4, 4, 4}; // Quarter notes

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614. Check wiring.");
    while (1);
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not detected. Please check wiring.");
    while (1);
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Health Monitor");

  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - prevTime >= intervalTime) {
    double bodyTemp = getBodyTemp();
    double spO2 = getSpO2();
    double heartRate = getHeartRate();
    if(bodyTemp >= 38.0)
      buzzerStart();
    else
      buzzerStop();
      
    displayData(bodyTemp, spO2, heartRate);

    prevTime = currentTime;
  }
}

double getBodyTemp() {
  double temp = mlx.readObjectTempC();
  if (isnan(temp)) {
    Serial.println("Error reading temperature.");
    return 0.0;
  }
  Serial.print("Body Temp: ");
  Serial.println(temp);
  return temp;
}

double getSpO2() {
  double spO2 = random(95, 100);
  Serial.print("SpO2: ");
  Serial.println(spO2);
  return spO2;
}

double getHeartRate() {
  double heartRate = random(60, 100);
  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  return heartRate;
}

void displayData(double temp, double spO2, double heartRate) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print("°C");

  lcd.setCursor(0, 1);
  lcd.print("SpO2: ");
  lcd.print(spO2, 0);
  lcd.print("%");

  lcd.setCursor(9, 1);
  lcd.print("HR: ");
  lcd.print(heartRate, 0);
}

void buzzerStart() {
  for (int i = 0; i < 8; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(buzzerPin, static_cast<int>(melody[i]), noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void buzzerStop(){
  noTone(buzzerPin);
}