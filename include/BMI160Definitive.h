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

// Gyroscope ranges
#define INTERNAL_GYRO_RANGE_CMD 0x43
#define INTERNAL_GYRO_RANGE_2000 0x00
#define INTERNAL_GYRO_RANGE_1000 0x01
#define INTERNAL_GYRO_RANGE_500 0x02
#define INTERNAL_GYRO_RANGE_250 0x03
#define INTERNAL_GYRO_RANGE_125 0x04

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
#define INTERNAL_GYRO_CONF_CMD 0x42
#define INTERNAL_GYRO_ODR_100HZ 0x08
#define INTERNAL_GYRO_ODR_1600HZ 0x0C
#define INTERNAL_GYRO_ODR_3200HZ 0xD

// Interrupts
#define INTERNAL_INTERRUPT_CONF_REG 0x51 
#define INTERNAL_INTR_ATTACH_CONF_REG 0x56
#define INTERNAL_INTERRUPT_DRDY_ENABLE 0x10 
#define INTERNAL_INTR_1_ATTACH_DRDY 0x80

#define INTERNAL_INT_OUT_CTRL_CONF_REG 0x53
#define INTERNAL_INT1_CONFIG 0xA

#define INTERNAL_INT_LATCH_REG 0x54
#define INTERNAL_INT_LATCH_TEMP_312_5_US 0x01


struct AccelerometerData {
    float x, y, z;
};

struct GyroscopeData {
    float x, y, z;
};

struct AccelGyroData {
    AccelerometerData accelerometer;
    GyroscopeData gyroscope;
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

enum GyroRanges {
    GYRO_RANGE_2000_DPS,
    GYRO_RANGE_1000_DPS,
    GYRO_RANGE_500_DPS,
    GYRO_RANGE_250_DPS,
    GYRO_RANGE_125_DPS,
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

typedef void (*INTRFunction)(void);

class BMI160Definitive {
    private:
        SPIClass *spiInstance;
        short sensorSelectPin;

        AccelRanges selectedAccelRange = ACCEL_2G;
        GyroRanges selectedGyroRange = GYRO_RANGE_2000_DPS;
    
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
        void setGyroRange(GyroRanges range);

        void setDataReadyInterrupt(INTRFunction intrCallback, short intrPin);
        
        AccelerometerData getAccelerometerData();
        GyroscopeData getGyroscopeData();
        
        AccelGyroData getAccelAndGyroData();

        uint8_t readRegister(uint8_t reg);
        void readRegisters(uint8_t reg, uint8_t *buffer, size_t numRegisters);
        void writeRegister(uint8_t reg, uint8_t value);
    };


#endif