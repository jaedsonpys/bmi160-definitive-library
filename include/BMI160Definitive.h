#ifndef BMI160DEFINITIVE_H
#define BMI160DEFINITIVE_H

#include <Arduino.h>
#include <SPI.h>

struct AccelerometerData {
    int x, y, z;
};

struct GyroscopeData {
    int x, y, z;
};


class BMI160Definitive {
    private:
        SPIClass spiInstance;

        uint8_t readRegister(uint8_t reg);
        void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
        void writeRegister(uint8_t reg, uint8_t value);
    
    public:
        void begin(SPIClass &spiInstance);

        AccelerometerData getAccelerometerData();
        AccelerometerData getGyroscopeData();
};

#endif