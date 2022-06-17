#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;


void angleConversion(int &currentAngle) {
  if (currentAngle < 0)
  {currentAngle+=360;}

  if (currentAngle > 360)
  {currentAngle-=360;}  }

void angleCalc(int &currentAngle){
  mpu.getEvent(&a, &g, &temp);
  double radianspersec = g.gyro.z;
  
    if (abs(radianspersec) > 0.1){
    double degrees = 0.1*radianspersec*180/3.14159;
    currentAngle = currentAngle + degrees;
    angleConversion(currentAngle);
    }
}

void gyroSetup()
{
    mpu.begin();
    if (!mpu.begin()) { //Check to ensure that the Gyroscope has started
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

