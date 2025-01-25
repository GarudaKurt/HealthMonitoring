#include "temperature.h"

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void initTemperature() {
   if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614. Check wiring.");
    while (1);
  }
}

double readTemperature() {
  double temp = mlx.readObjectTempC();
  return temp;
}
