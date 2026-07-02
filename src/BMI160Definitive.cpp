#include <Arduino.h>
#include <BMI160Definitive.h>

#include <SPI.h>

SPISettings BMI160SPISettings(10000000, MSBFIRST, SPI_MODE0);

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

    vTaskDelay(pdMS_TO_TICKS(10));
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
    uint8_t gyroPowerMode = pmuStatus >> 2 & 0x03;

    GyroPowerModes definedPMU;

    if(gyroPowerMode == (uint8_t)PMU_STATUS_SUSPEND) {
        definedPMU = GYRO_SUSPEND_POWER_MODE;
    } else if(gyroPowerMode == (uint8_t)PMU_STATUS_NORMAL) {
        definedPMU = GYRO_NORMAL_POWER_MODE;
    } else if(gyroPowerMode == (uint8_t)PMU_STATUS_FAST_STARTUP) {
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
        currentPowerMode = this->getAccelPowerMode();
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
        currentPowerMode = this->getGyroPowerMode();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI160Definitive::setAccelDataRate(AccelDataRate dataRate) {
    uint8_t currentConf = this->readRegister(INTERNAL_ACCEL_CONF_CMD);
    uint8_t selectedODR;

    switch (dataRate) {
        case ACCEL_100HZ_DATA_RATE:
            selectedODR = INTERNAL_ACCEL_ODR_100HZ;
            break;
        case ACCEL_800HZ_DATA_RATE:
            selectedODR = INTERNAL_ACCEL_ODR_800HZ;
            break;
        case ACCEL_1600HZ_DATA_RATE:
            selectedODR = INTERNAL_ACCEL_ODR_1600HZ;
            break;
    }

    uint8_t newConf = currentConf | selectedODR;
    this->writeRegister(INTERNAL_ACCEL_CONF_CMD, newConf);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t updatedConf = this->readRegister(INTERNAL_ACCEL_CONF_CMD);
    while(updatedConf != newConf) {
        updatedConf = this->readRegister(INTERNAL_ACCEL_CONF_CMD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI160Definitive::setGyroDataRate(GyroDataRate dataRate) {
    uint8_t currentConf = this->readRegister(INTERNAL_GYRO_CONF_CMD);
    uint8_t selectedODR;

    switch (dataRate) {
        case GYRO_100HZ_DATA_RATE:
            selectedODR = INTERNAL_GYRO_ODR_100HZ;
            break;
        case GYRO_1600HZ_DATA_RATE:
            selectedODR = INTERNAL_GYRO_ODR_1600HZ;
            break;
        case GYRO_3200HZ_DATA_RATE:
            selectedODR = INTERNAL_GYRO_ODR_3200HZ;
            break;
    }

    uint8_t newConf = currentConf | selectedODR;
    this->writeRegister(INTERNAL_GYRO_CONF_CMD, newConf);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t updatedConf = this->readRegister(INTERNAL_GYRO_CONF_CMD);
    while(updatedConf != newConf) {
        updatedConf = this->readRegister(INTERNAL_GYRO_CONF_CMD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI160Definitive::setGyroRange(GyroRanges range) {
    uint8_t choosedRange;

    switch (range) {
        case GYRO_RANGE_2000_DPS:
            this->writeRegister(INTERNAL_GYRO_RANGE_CMD, INTERNAL_GYRO_RANGE_2000);
            choosedRange = INTERNAL_GYRO_RANGE_2000;
            this->selectedGyroRange = GYRO_RANGE_2000_DPS;
            break;
        case GYRO_RANGE_1000_DPS:
            this->writeRegister(INTERNAL_GYRO_RANGE_CMD, INTERNAL_GYRO_RANGE_1000);
            choosedRange = INTERNAL_GYRO_RANGE_1000;
            this->selectedGyroRange = GYRO_RANGE_1000_DPS;
            break;
        case GYRO_RANGE_500_DPS:
            this->writeRegister(INTERNAL_GYRO_RANGE_CMD, INTERNAL_GYRO_RANGE_500);
            choosedRange = INTERNAL_GYRO_RANGE_500;
            this->selectedGyroRange = GYRO_RANGE_500_DPS;
            break;
        case GYRO_RANGE_250_DPS:
            this->writeRegister(INTERNAL_GYRO_RANGE_CMD, INTERNAL_GYRO_RANGE_250);
            choosedRange = INTERNAL_GYRO_RANGE_250;
            this->selectedGyroRange = GYRO_RANGE_250_DPS;
            break;
        case GYRO_RANGE_125_DPS:
            this->writeRegister(INTERNAL_GYRO_RANGE_CMD, INTERNAL_GYRO_RANGE_125);
            choosedRange = INTERNAL_GYRO_RANGE_125;
            this->selectedGyroRange = GYRO_RANGE_125_DPS;
            break;
    };

    vTaskDelay(pdMS_TO_TICKS(10));
    uint8_t selectedRange = this->readRegister(INTERNAL_GYRO_RANGE_CMD);

    while(selectedRange != choosedRange) {
        selectedRange = this->readRegister(INTERNAL_GYRO_RANGE_CMD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void BMI160Definitive::setAccelRange(AccelRanges range) {
    uint8_t choosedRange;

    switch (range) {
        case ACCEL_2G:
            this->writeRegister(INTERNAL_ACCEL_RANGE_CMD, INTERNAL_ACCEL_RANGE_2G);
            choosedRange = INTERNAL_ACCEL_RANGE_2G;
            this->selectedAccelRange = ACCEL_2G;
            break;
        case ACCEL_4G:
            this->writeRegister(INTERNAL_ACCEL_RANGE_CMD, INTERNAL_ACCEL_RANGE_4G);
            choosedRange = INTERNAL_ACCEL_RANGE_4G;
            this->selectedAccelRange = ACCEL_4G;
            break;
        case ACCEL_8G:
            this->writeRegister(INTERNAL_ACCEL_RANGE_CMD, INTERNAL_ACCEL_RANGE_8G);
            choosedRange = INTERNAL_ACCEL_RANGE_8G;
            this->selectedAccelRange = ACCEL_8G;
            break;
        case ACCEL_16G:
            this->writeRegister(INTERNAL_ACCEL_RANGE_CMD, INTERNAL_ACCEL_RANGE_16G);
            choosedRange = INTERNAL_ACCEL_RANGE_16G;
            this->selectedAccelRange = ACCEL_16G;
            break;
    };

    vTaskDelay(pdMS_TO_TICKS(10));
    uint8_t selectedRange = this->readRegister(INTERNAL_ACCEL_RANGE_CMD);

    while(selectedRange != choosedRange) {
        selectedRange = this->readRegister(INTERNAL_ACCEL_RANGE_CMD);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int16_t BMI160Definitive::read16BitsRegister(uint8_t lsbReg, uint8_t msbReg) {
    uint8_t msbRegValue;
    uint8_t lsbRegValue;

    this->spiInstance->beginTransaction(BMI160SPISettings);
    digitalWrite(this->sensorSelectPin, LOW);

    this->spiInstance->transfer(msbReg | 0x80);
    msbRegValue = this->spiInstance->transfer(lsbReg | 0x80);
    lsbRegValue = this->spiInstance->transfer(0x00);

    digitalWrite(this->sensorSelectPin, HIGH);
    this->spiInstance->endTransaction();

    int16_t fullValue = (int16_t)(msbRegValue << 8) | lsbRegValue;
    return fullValue;
}

AccelerometerData BMI160Definitive::getAccelerometerData() {
    int16_t xAxisRaw = this->read16BitsRegister(0x12, 0x13);
    int16_t yAxisRaw = this->read16BitsRegister(0x14, 0x15);
    int16_t zAxisRaw = this->read16BitsRegister(0x16, 0x17);

    AccelerometerData gData;
    float lsbPerG;

    switch (this->selectedAccelRange) {
        case ACCEL_2G:
            lsbPerG = 16384.0f;
            break;
        case ACCEL_4G:
            lsbPerG = 8192.0f;
            break;
        case ACCEL_8G:
            lsbPerG = 4096.0f;
            break;
        case ACCEL_16G:
            lsbPerG = 2048.0f;
            break;
    }

    gData.x = (float)xAxisRaw / lsbPerG;
    gData.y = (float)yAxisRaw / lsbPerG;
    gData.z = (float)zAxisRaw / lsbPerG;

    return gData;
}

void BMI160Definitive::setDataReadyInterrupt(INTRFunction intrCallback, short intrPin) {
    this->writeRegister(INTERNAL_INTERRUPT_CONF_REG, INTERNAL_INTERRUPT_DRDY_ENABLE);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t intrConf = this->readRegister(INTERNAL_INTERRUPT_CONF_REG);

    while(intrConf != INTERNAL_INTERRUPT_DRDY_ENABLE) {
        intrConf = this->readRegister(INTERNAL_INTERRUPT_CONF_REG);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    this->writeRegister(INTERNAL_INTR_ATTACH_CONF_REG, INTERNAL_INTR_1_ATTACH_DRDY);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t intrAttachConf = this->readRegister(INTERNAL_INTR_ATTACH_CONF_REG);

    while(intrAttachConf != INTERNAL_INTR_1_ATTACH_DRDY) {
        intrAttachConf = this->readRegister(INTERNAL_INTR_ATTACH_CONF_REG);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    this->writeRegister(INTERNAL_INT_OUT_CTRL_CONF_REG, INTERNAL_INT1_CONFIG);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t intrOutConf = this->readRegister(INTERNAL_INT_OUT_CTRL_CONF_REG);

    while(intrOutConf != INTERNAL_INT1_CONFIG) {
        intrAttachConf = this->readRegister(INTERNAL_INT_OUT_CTRL_CONF_REG);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    attachInterrupt(digitalPinToInterrupt(intrPin), intrCallback, HIGH);
}
