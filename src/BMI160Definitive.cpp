#include <Arduino.h>
#include <BMI160Definitive.h>

#include <SPI.h>

void BMI160Definitive::begin(SPIClass &spiInstance) {
    this->spiInstance = spiInstance;
}