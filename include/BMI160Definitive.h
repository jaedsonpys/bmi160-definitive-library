#ifndef BMI160DEFINITIVE_H
#define BMI160DEFINITIVE_H

#include <Arduino.h>
#include <SPI.h>

// Power modes
#define INTERNAL_ACCEL_NORMAL_POWER_MODE 0x11
#define INTERNAL_ACCEL_LOW_POWER_MODE 0x12
#define  INTERNAL_ACCEL_SUSPEND_POWER_MODE 0x10

#define INTERNAL_GYRO_NORMAL_POWER_MODE 0x15
#define INTERNAL_GYRO_SUSPEND_POWER_MODE 0x14
#define INTERNAL_GYRO_FAST_STARTUP_POWER_MODE 0x17

struct AccelerometerData {
    int x, y, z;
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


class BMI160Definitive {
    private:
        SPIClass *spiInstance;
        short sensorSelectPin;
    
    public:
        bool begin(SPIClass *spiInstance, short sensorCS);
        
        AccelPowerModes getAccelPowerMode();
        GyroPowerModes getGyroPowerMode();

        void setAccelPowerMode(AccelPowerModes powerMode);
        void setGyroPowerMode(GyroPowerModes powerMode);
        
        AccelerometerData getAccelerometerData();
        AccelerometerData getGyroscopeData();
        
        uint8_t readRegister(uint8_t reg);
        void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
        void writeRegister(uint8_t reg, uint8_t value);
    };


#endif