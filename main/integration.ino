#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal_I2C lcd(0x27, 16, 2);
MAX30105 particleSensor;

#define ECG_PIN A0 //connect to A0 pin
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

int graphData[16] = {0}; // Array for graph data

//vertical graph bars
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

//SPO2 sensors
// Buffers for IR and Red LED data
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
uint16_t irBuffer[100]; // Infrared LED sensor data
uint16_t redBuffer[100]; // Red LED sensor data
#else
uint32_t irBuffer[100]; 
uint32_t redBuffer[100];
#endif

int32_t spo2; // SpO2 value
int8_t validSPO2; // Indicator for valid SpO2 value
int32_t bufferLength = 100; // Number of samples to store in the buffer


void setup() {
  Serial.begin(9600);
  Serial.setTimeOut(100);
  pinMode(buzzerPin, OUTPUT);
  // Initialize the MLX90614 sensor
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614. Check wiring.");
    while (1);
  }

  // Initialize the MAX30105 sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not detected. Please check wiring.");
    while (1); // Stop execution if sensor initialization fails
  }

  // Sensor configuration
  byte ledBrightness = 60; // LED brightness
  byte sampleAverage = 4; // Number of samples to average
  byte ledMode = 2; // Red and IR LEDs enabled
  byte sampleRate = 100; // Sampling rate (in Hz)
  int pulseWidth = 411; // LED pulse width
  int adcRange = 4096; // ADC range

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  lcd.init();
  lcd.backlight();

  // Create custom characters for graph levels
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, bar[i]);
  }

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

    if (bodyTemp >= 38.0) {
      buzzerStart();
    } else {
      buzzerStop();
    }

    if(spO2 <= 30.0)
      buzzerStart();
    else
      buzzerStop();

    displayData(bodyTemp, spO2, heartRate);
    displayHeartRateGraph(heartRate);

    prevTime = currentTime;
  }
}

double getBodyTemp() {
  //double temp = mlx.readObjectTempC();
  double temp = random(30, 40);
  if (isnan(temp)) {
    Serial.println("Error reading temperature.");
    return 0.0;
  }
  Serial.print("Body Temp: ");
  Serial.println(temp);
  return temp;
}

double getSpO2() {
  // Fill the buffer with initial readings
  for (int i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false)
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, nullptr, nullptr);

  if (validSPO2) {
    return spo2;
  } else {
    return -1.0;
  }
}

double getHeartRate() {
  //double heartRate = random(60, 100);
  double hearRate = analogRead(ECG_PIN)
  Serial.print(heartRate);
  Serial.println("° C");
  return heartRate;
}

void displayData(double temp, double spO2, double heartRate) {
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

void displayHeartRateGraph(double heartRate) {
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

void buzzerStart() {
  for (int i = 0; i < 8; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(buzzerPin, static_cast<int>(melody[i]), noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void buzzerStop() {
  noTone(buzzerPin);
}
