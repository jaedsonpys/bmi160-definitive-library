#include <Arduino.h>
#include <BMI160Definitive.h>

#include <SPI.h>

SPISettings BMI160SPISettings(10000000, MSBFIRST, SPI_MODE0);

bool BMI160Definitive::begin(SPIClass *spiInstance, short sensorCS) {
    this->spiInstance = spiInstance;
    this->sensorSelectPin = sensorCS;

    this->spiInstance->beginTransaction(BMI160SPISettings);

    pinMode(this->sensorSelectPin, OUTPUT);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(0x7F);
    vTaskDelay(10);
    this->spiInstance->transfer(0x00);

    digitalWrite(this->sensorSelectPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t chipID = this->readRegister(0x00);
    return chipID == 0xD1;
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

AccelPowerModes BMI160Definitive::getAccelPowerMode() {
    uint8_t pmuStatus = this->readRegister(0x03);
    uint8_t accelPowerMode = pmuStatus >> 4 & 0x03;

    AccelPowerModes definedPMU;

    if(accelPowerMode == INTERNAL_ACCEL_SUSPEND_POWER_MODE) {
        definedPMU = ACCEL_SUSPEND_POWER_MODE;
    } else if(accelPowerMode == INTERNAL_ACCEL_LOW_POWER_MODE) {
        definedPMU = ACCEL_LOW_POWER_MODE;
    } else if(accelPowerMode == INTERNAL_ACCEL_NORMAL_POWER_MODE) {
        definedPMU = ACCEL_NORMAL_POWER_MODE;
    }

    return definedPMU;
}

GyroPowerModes BMI160Definitive::getGyroPowerMode() {
    uint8_t pmuStatus = this->readRegister(0x03);
    uint8_t accelPowerMode = pmuStatus >> 2 & 0x03;

    GyroPowerModes definedPMU;

    if(accelPowerMode == INTERNAL_ACCEL_SUSPEND_POWER_MODE) {
        definedPMU = GYRO_SUSPEND_POWER_MODE;
    } else if(accelPowerMode == INTERNAL_GYRO_NORMAL_POWER_MODE) {
        definedPMU = GYRO_NORMAL_POWER_MODE;
    } else if(accelPowerMode == INTERNAL_GYRO_FAST_STARTUP_POWER_MODE) {
        definedPMU = GYRO_FAST_STARTUP_POWER_MODE;
    }

    return definedPMU;
}

void BMI160Definitive::setAccelPowerMode(AccelPowerModes powerMode) {
    switch(powerMode) {
        case ACCEL_NORMAL_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_ACCEL_NORMAL_POWER_MODE);
            break;
        case ACCEL_SUSPEND_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_ACCEL_SUSPEND_POWER_MODE);
            break;
        case ACCEL_LOW_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_ACCEL_LOW_POWER_MODE);
            break;
        default:
            this->writeRegister(0x7E, INTERNAL_ACCEL_NORMAL_POWER_MODE);
            break;
    };

    vTaskDelay(pdMS_TO_TICKS(10));
    AccelPowerModes currentPowerMode = this->getAccelPowerMode();

    while(currentPowerMode != powerMode) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI160Definitive::setGyroPowerMode(GyroPowerModes powerMode) {
    switch(powerMode) {
        case GYRO_NORMAL_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_GYRO_NORMAL_POWER_MODE);
            break;
        case GYRO_SUSPEND_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_GYRO_SUSPEND_POWER_MODE);
            break;
        case GYRO_FAST_STARTUP_POWER_MODE:
            this->writeRegister(0x7E, INTERNAL_GYRO_FAST_STARTUP_POWER_MODE);
            break;
        default:
            this->writeRegister(0x7E, INTERNAL_GYRO_NORMAL_POWER_MODE);
            break;
    };

    vTaskDelay(pdMS_TO_TICKS(10));
    GyroPowerModes currentPowerMode = this->getGyroPowerMode();

    while(currentPowerMode != powerMode) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}