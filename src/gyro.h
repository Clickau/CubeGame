#pragma once
#include <Adafruit_MPU6050.h>


const int mpu_pin_scl = 32;
const int mpu_pin_sda = 33;
Adafruit_MPU6050 mpu;

// returns the index (0-5) of the side that is up or -1 if it's currently moving
int gyro_get_orientation_internal()
{
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);

    // Serial.printf("x: %f, y: %f, z: %f\n", a.acceleration.x, a.acceleration.y, a.acceleration.z);

    bool x = abs(a.acceleration.x) < 5;
    bool y = abs(a.acceleration.y) < 5;
    bool z = abs(a.acceleration.z) < 5;

    bool x1 = abs(a.acceleration.x) > 5;
    bool y1 = abs(a.acceleration.y) > 5;
    bool z1 = abs(a.acceleration.z) > 5;

    // Serial.printf("x: %d, y: %d, z: %d, x1: %d, y1: %d, z1: %d\n", x, y, z, x1, y1, z1);

    if (x && y && z1 && a.acceleration.z > 0)
        return 0;
    if (x && y && z1 && a.acceleration.z < 0)
        return 5;
    if (x && z && y1 && a.acceleration.y > 0)
        return 4;
    if (x && z && y1 && a.acceleration.y < 0)
        return 1;
    if (y && z && x1 && a.acceleration.x > 0)
        return 3;
    if (y && z && x1 && a.acceleration.x < 0)
        return 2;
    
    return -1;
}

int gyro_get_orientation()
{
    int internal = gyro_get_orientation_internal();
    if (internal == 0) return 4;
    if (internal == 1) return 2;
    if (internal == 2) return 5;
    if (internal == 3) return 0;
    if (internal == 4) return 3;
    if (internal == 5) return 1;
    return -1;
}

void gyro_setup()
{
    Wire.begin(mpu_pin_sda, mpu_pin_scl);
    mpu.begin();
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}