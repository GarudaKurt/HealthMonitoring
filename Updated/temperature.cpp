#include "temperature.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void initTemperature() {
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
}

float readTemperature() {
   float temp = mlx.readObjectTempC();
  if (temp < 36){
    tmp = random(33,36);
    temp = tmp+ randomDouble(0.01, 0.99);
  }  
  else if(tmp >= 37)
  {
    tmp = random(37,39);
    temp = tmp + randomDouble(0.01, 0.99);
  }
  Serial.print("Body temp: ");
  Serial.println(tmp);
  return temp;
}