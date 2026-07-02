#ifndef BMI160DEFINITIVE_H
#define BMI160DEFINITIVE_H

#include <Arduino.h>
#include <SPI.h>

// Power modes
#define INTERNAL_ACCEL_SUSPEND_POWER_MODE 0x10
#define INTERNAL_ACCEL_NORMAL_POWER_MODE 0x11
#define INTERNAL_ACCEL_LOW_POWER_MODE 0x12

#define INTERNAL_GYRO_SUSPEND_POWER_MODE 0x14
#define INTERNAL_GYRO_NORMAL_POWER_MODE 0x15
#define INTERNAL_GYRO_FAST_STARTUP_POWER_MODE 0x17

// Accelerometer ranges
#define INTERNAL_ACCEL_RANGE_CMD 0x41

#define INTERNAL_ACCEL_RANGE_2G 0x03
#define INTERNAL_ACCEL_RANGE_4G 0x05
#define INTERNAL_ACCEL_RANGE_8G 0x08
#define INTERNAL_ACCEL_RANGE_16G 0x0C

// PMU status
#define PMU_STATUS_SUSPEND 0x00
#define PMU_STATUS_NORMAL 0x01
#define PMU_STATUS_LOW_POWER 0x02
#define PMU_STATUS_FAST_STARTUP 0x03

// Accelerometer configuration commands
#define INTERNAL_ACCEL_CONF_CMD 0x40
#define INTERNAL_ACCEL_ODR_100HZ 0x08
#define INTERNAL_ACCEL_ODR_800HZ 0x0B
#define INTERNAL_ACCEL_ODR_1600HZ 0x0C

// Gyroscope configuration commands
#define INTERNAL_GYRO_CONF_CMD 0x40
#define INTERNAL_GYRO_ODR_100HZ 0x08
#define INTERNAL_GYRO_ODR_1600HZ 0x0C
#define INTERNAL_GYRO_ODR_3200HZ 0xD

struct AccelerometerData {
    float x, y, z;
};

struct GyroscopeData {
    int x, y, z;
};

enum AccelPowerModes {
    ACCEL_NORMAL_POWER_MODE,
    ACCEL_SUSPEND_POWER_MODE,
    ACCEL_LOW_POWER_MODE
};

enum GyroPowerModes {
    GYRO_NORMAL_POWER_MODE,
    GYRO_SUSPEND_POWER_MODE,
    GYRO_FAST_STARTUP_POWER_MODE
};

enum AccelRanges {
    ACCEL_2G,
    ACCEL_4G,
    ACCEL_8G,
    ACCEL_16G,
};

enum AccelDataRate {
    ACCEL_100HZ_DATA_RATE,
    ACCEL_800HZ_DATA_RATE,
    ACCEL_1600HZ_DATA_RATE
};

enum GyroDataRate {
    GYRO_100HZ_DATA_RATE,
    GYRO_1600HZ_DATA_RATE,
    GYRO_3200HZ_DATA_RATE
};

class BMI160Definitive {
    private:
        SPIClass *spiInstance;
        short sensorSelectPin;
        AccelRanges selectedAccelRange = AccelRanges::ACCEL_2G;

        int16_t read16BitsRegister(uint8_t msbReg, uint8_t lsbReg);
    
    public:
        bool begin(SPIClass *spiInstance, short miso, short mosi, short sck, short cs);
        void softReset();
        
        AccelPowerModes getAccelPowerMode();
        GyroPowerModes getGyroPowerMode();

        void setAccelPowerMode(AccelPowerModes powerMode);
        void setGyroPowerMode(GyroPowerModes powerMode);
        void setAccelDataRate(AccelDataRate dataRate);
        void setGyroDataRate(GyroDataRate dataRate);
        void setAccelRange(AccelRanges range);
        
        AccelerometerData getAccelerometerData();
        AccelerometerData getGyroscopeData();
        
        uint8_t readRegister(uint8_t reg);
        void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
        void writeRegister(uint8_t reg, uint8_t value);
    };


#endif