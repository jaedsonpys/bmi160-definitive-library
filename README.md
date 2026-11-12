# BMI160 Accelerometer & Gyroscope sensor library

Tired of using giant and old BMI160 libraries for Arduino/Espresif boards, I decide to create my own library with a modern code, focused on efficiency and simplicity. All development was based on the original [BMI160 Bosch Sensor datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi160-ds000.pdf). If you want collaborate with your code, please base your known on this datasheet.



## Important library features

- **Sensor calibration**: Use the `BMI160.calibrateBMI160` function to execute the *Fast Offset Calibration* from BMI160.

- **SPI Communication**: Set your prefered SPI settings and pins. The library doesn't control what pins you should use.

```cpp
#include <BMI160Definitive.h>

#define IMU_CS_PIN 21
#define IMU_MISO_PIN 5
#define IMU_MOSI_PIN 19
#define IMU_SCK_PIN 18

BMI160Definitive BMI160;
SPIClass *bmi160SPI = new SPIClass();

void setup() {
  bool started = BMI160.begin(bmi160SPI, IMU_MISO_PIN, IMU_MOSI_PIN, IMU_SCK_PIN, IMU_CS_PIN);
}
```

- **Data Ready Interrupt (DRDY)**: Call a function always that the BMI160 has a new data ready (accelerometer and gyroscope).

> Only the INT1 pin from can use the Data Ready interrupt!

```cpp
#include <BMI160Definitive.h>

#define IMU_CS_PIN 21
#define IMU_MISO_PIN 5
#define IMU_MOSI_PIN 19
#define IMU_SCK_PIN 18

#define INTERRUPT_DRDY_PIN 14

BMI160Definitive BMI160;
SPIClass *bmi160SPI = new SPIClass();

int counter = 0;

void IRAM_ATTR myInterrupt() {
    counter++;
}

void setup() {
  bool started = BMI160.begin(bmi160SPI, IMU_MISO_PIN, IMU_MOSI_PIN, IMU_SCK_PIN, IMU_CS_PIN);

  if(started) {
    BMI160.setDataReadyInterrupt(&myInterrupt, INTERRUPT_DRDY_PIN);
  }
}
```

## See a example

You can find more usage examples in the [examples directory](https://github.com/jaedsonpys/bmi160-definitive-library/tree/master/examples) on GitHub. Fell free to add new examples files there!

```cpp
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
```

## Trademark Notice

Bosch® is a registered trademark of Robert Bosch GmbH, Germany. All rights, titles, and interests in the Bosch name, trademarks, logos, and related intellectual property belong to their respective owners.

This library is an independent, community-developed software project created to provide support for the **BMI160**. It is not affiliated with, endorsed by, sponsored by, or officially associated with Robert Bosch GmbH or any of its subsidiaries.

The use of the Bosch name and sensor model designation is solely for identification and compatibility purposes. No ownership of, or rights to, Bosch trademarks or other intellectual property are claimed or implied.

All trademarks and product names mentioned in this project belong to their respective owners.

## License

This project uses **Apache License**. Please see the [license file](https://github.com/jaedsonpys/bmi160-definitive-library/blob/master/LICENSE) before use the library or clone for any modifications.

```txt
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
```