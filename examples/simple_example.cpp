#include <Arduino.h>
#include <BMI160Definitive.h>
#include <SPI.h>

#define IMU_CS_PIN 21
#define IMU_MISO_PIN 5
#define IMU_MOSI_PIN 19
#define IMU_SCK_PIN 18

int counter = 0;

BMI160Definitive BMI160;
SPIClass *bmi160SPI = new SPIClass();

void IRAM_ATTR bmi160Interrupt() {
  counter++;
}

void setup() {
  Serial.begin(115200);
  bool started = BMI160.begin(bmi160SPI, IMU_MISO_PIN, IMU_MOSI_PIN, IMU_SCK_PIN, IMU_CS_PIN);

  while(!started) {
    Serial.println("Failed to connect");
    delay(1000);
  }

  BMI160.setAccelPowerMode(AccelPowerModes::ACCEL_NORMAL_POWER_MODE);
  BMI160.setGyroPowerMode(GyroPowerModes::GYRO_NORMAL_POWER_MODE);
  BMI160.setAccelDataRate(AccelDataRate::ACCEL_1600HZ_DATA_RATE);
  BMI160.setGyroDataRate(GyroDataRate::GYRO_1600HZ_DATA_RATE);
  BMI160.setAccelRange(AccelRanges::ACCEL_16G);
  BMI160.setGyroRange(GyroRanges::GYRO_RANGE_2000_DPS);

  Serial.println("Setting DRDY Interrupt");
  BMI160.setDataReadyInterrupt(&bmi160Interrupt, 14);

  Serial.println("Calibration will start in 2 seconds...");
  delay(2000);

  Serial.println("Calibrating...");
  BMI160.calibrateBMI160();
  Serial.println("Calibrated!");
}

void loop() {
  AccelGyroData unifiedData = BMI160.getAccelAndGyroData();

  AccelerometerData accelData = unifiedData.accelerometer;
  Serial.print(">X:" + String(accelData.x));
  Serial.print(",Y:" + String(accelData.y));
  Serial.println(",Z:" + String(accelData.z));

  GyroscopeData gyroData = unifiedData.gyroscope;
  Serial.print(">GX:" + String(gyroData.x));
  Serial.print(",GY:" + String(gyroData.y));
  Serial.println(",GZ:" + String(gyroData.z));

  Serial.println("Interrupt count: " + String(counter));
  
  delay(1000);
}