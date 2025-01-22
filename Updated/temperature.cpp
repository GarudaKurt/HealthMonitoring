#include "temperature.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

// Pointer to the MLX90614 sensor object
Adafruit_MLX90614 *mlx = nullptr;

void initTemperature() {
    // Allocate memory for the MLX90614 object on the heap
    mlx = new Adafruit_MLX90614();

    if (!mlx->begin()) {
        Serial.println("Error connecting to MLX sensor. Check wiring.");
        // Clean up and halt execution
        delete mlx;
        mlx = nullptr;
        while (1);
    }
}

float readTemperature() {
    if (!mlx) {
        Serial.println("MLX sensor not initialized!");
        return -1; // Return an error value
    }

    float temp = mlx->readObjectTempC();
    Serial.print("Body temp: ");
    Serial.println(temp);
    return temp;
}

void cleanupTemperature() {
    // Free dynamically allocated memory
    if (mlx) {
        delete mlx;
        mlx = nullptr;
    }
}
