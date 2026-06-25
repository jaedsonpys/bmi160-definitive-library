#include <Arduino.h>
#include <BMI160Definitive.h>

#include <SPI.h>

SPISettings BMI160SPISettings(10000000, MSBFIRST, SPI_MODE0);

void BMI160Definitive::begin(SPIClass *spiInstance, short sensorCS) {
    this->spiInstance = spiInstance;
    this->sensorSelectPin = sensorCS;

    this->spiInstance->beginTransaction(BMI160SPISettings);

    pinMode(this->sensorSelectPin, OUTPUT);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(0x7F);
    this->spiInstance->transfer(0x00);

    digitalWrite(this->sensorSelectPin, HIGH);

    vTaskDelay(pdMS_TO_TICKS(10));
}

uint8_t BMI160Definitive::readRegister(uint8_t reg) {
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg | 0x80);
    uint8_t regReaded = this->spiInstance->transfer(0x00);
    
    digitalWrite(this->sensorSelectPin, HIGH);

    return regReaded;
}

void BMI160Definitive::writeRegister(uint8_t reg, uint8_t value) {
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg & 0x7F);
    this->spiInstance->transfer(value);

    digitalWrite(this->sensorSelectPin, HIGH);
}

void BMI160Definitive::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg | 0x80);

    for(int i = 0; i < len; i++) {
        buffer[i] = this->spiInstance->transfer(0x00);
    }

    digitalWrite(this->sensorSelectPin, HIGH);
}