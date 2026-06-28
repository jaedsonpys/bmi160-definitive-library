#include <Arduino.h>
#include <BMI160Definitive.h>

#include <SPI.h>

SPISettings BMI160SPISettings(1000000, MSBFIRST, SPI_MODE0);

bool BMI160Definitive::begin(SPIClass *spiInstance, short miso, short mosi, short sck, short cs) {
    this->spiInstance = spiInstance;
    this->sensorSelectPin = cs;

    pinMode(this->sensorSelectPin, OUTPUT);
    digitalWrite(this->sensorSelectPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));

    bool spiStarted = this->spiInstance->begin(sck, miso, mosi);
    
    if(!spiStarted) {
        return false;
    }

    this->softReset();
    this->readRegister(0x7F);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t chipID = this->readRegister(0x00);
    return chipID == 0xD1;
}

void BMI160Definitive::softReset() {
    this->writeRegister(0x7E, 0xB6);
    vTaskDelay(pdMS_TO_TICKS(100));
}

uint8_t BMI160Definitive::readRegister(uint8_t reg) {
    this->spiInstance->beginTransaction(BMI160SPISettings);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg | 0x80);
    uint8_t regReaded = this->spiInstance->transfer(0x00);
    
    digitalWrite(this->sensorSelectPin, HIGH);
    this->spiInstance->endTransaction();

    return regReaded;
}

void BMI160Definitive::writeRegister(uint8_t reg, uint8_t value) {
    this->spiInstance->beginTransaction(BMI160SPISettings);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg & 0x7F);
    this->spiInstance->transfer(value);

    digitalWrite(this->sensorSelectPin, HIGH);
    this->spiInstance->endTransaction();
}

void BMI160Definitive::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    this->spiInstance->beginTransaction(BMI160SPISettings);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(reg | 0x80);

    for(int i = 0; i < len; i++) {
        buffer[i] = this->spiInstance->transfer(0x00);
    }

    digitalWrite(this->sensorSelectPin, HIGH);
    this->spiInstance->endTransaction();
}

AccelPowerModes BMI160Definitive::getAccelPowerMode() {
    uint8_t pmuStatus = this->readRegister(0x03);
    uint8_t accelPowerMode = pmuStatus >> 4 & 0x03;

    AccelPowerModes definedPMU;

    if(accelPowerMode == (uint8_t)PMU_STATUS_SUSPEND) {
        definedPMU = ACCEL_SUSPEND_POWER_MODE;
    } else if(accelPowerMode == (uint8_t)PMU_STATUS_LOW_POWER) {
        definedPMU = ACCEL_LOW_POWER_MODE;
    } else if(accelPowerMode == (uint8_t)PMU_STATUS_NORMAL) {
        definedPMU = ACCEL_NORMAL_POWER_MODE;
    }

    return definedPMU;
}

GyroPowerModes BMI160Definitive::getGyroPowerMode() {
    uint8_t pmuStatus = this->readRegister(0x03);
    uint8_t accelPowerMode = pmuStatus >> 2 & 0x03;

    GyroPowerModes definedPMU;

    if(accelPowerMode == (uint8_t)PMU_STATUS_SUSPEND) {
        definedPMU = GYRO_SUSPEND_POWER_MODE;
    } else if(accelPowerMode == (uint8_t)PMU_STATUS_NORMAL) {
        definedPMU = GYRO_NORMAL_POWER_MODE;
    } else if(accelPowerMode == (uint8_t)PMU_STATUS_FAST_STARTUP) {
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
        AccelPowerModes currentPowerMode = this->getAccelPowerMode();
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
        GyroPowerModes currentPowerMode = this->getGyroPowerMode();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

AccelerometerData BMI160Definitive::getAccelerometerData() {
    uint8_t xAxisRaw = this->readRegister(0x13);
    uint8_t yAxisRaw = this->readRegister(0x15);
    uint8_t zAxisRaw = this->readRegister(0x17);

    AccelerometerData gData;

    gData.x = (float)xAxisRaw / 16384.0f;
    gData.y = (float)yAxisRaw / 16384.0f;
    gData.z = (float)zAxisRaw / 16384.0f;

    return gData;
}