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
#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid

byte pulseLED = 11; //Must be on PWM pin
byte readLED = 13; //Blinks with each data read

void setup() {
  Serial.begin(115200);
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

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

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
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps
  unsigned long currentTime = millis();

  if (currentTime - prevTime >= intervalTime) {
    double bodyTemp = getBodyTemp();
    double heartRate = getHeartRate();

    if (bodyTemp >= 38.0) {
      buzzerStart();
    } else {
      buzzerStop();
    }

    //read the first 100 samples, and determine the signal range
    for (byte i = 0 ; i < bufferLength ; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      // Serial.print(F("red="));
      // Serial.print(redBuffer[i], DEC);
      // Serial.print(F(", ir="));
      // Serial.println(irBuffer[i], DEC);
    }

    //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, nullptr, nullptr);

    //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample


      // Serial.print(F(", SPO2="));
      // Serial.print(spo2, DEC);

      // Serial.print(F(", SPO2Valid="));
      // Serial.println(validSPO2, DEC);
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, nullptr, nullptr);

    if(validSPO2 == 0)
      spo2 = -1;

    displayData(bodyTemp, spo2, heartRate);
    displayHeartRateGraph(heartRate);

    prevTime = currentTime;
  }
}

double getBodyTemp() {
  //double temp = mlx.readObjectTempC();
  // double temp = random(30, 40);
  // if (isnan(temp)) {
  //   Serial.println("Error reading temperature.");
  //   return 0.0;
  // }
  // Serial.print("Body Temp: ");
  // Serial.println(temp);
  double temperature = particleSensor.readTemperature();

  Serial.print("temperatureC=");
  Serial.print(temperature, 4);
  return temperature;
}


double getHeartRate() {
  //double heartRate = random(60, 100);
  double hearRate = analogRead(ECG_PIN)
  Serial.print(heartRate);
  Serial.println("° C");
  return heartRate;
}

void displayData(double temp, int32_t spO2, double heartRate) {
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
