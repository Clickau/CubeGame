#include <Adafruit_MPU6050.h>


// MPU with scl=22, sda=21
Adafruit_MPU6050 mpu;

enum class Orientation
{
    Horizontal,
    Vertical,
    Sideways,
    HorizontalInverse,
    VerticalInverse,
    SidewaysInverse,
    None
};

Orientation getOrientation()
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
        return Orientation::Horizontal;
    if (x && y && z1 && a.acceleration.z < 0)
        return Orientation::HorizontalInverse;
    if (x && z && y1 && a.acceleration.y > 0)
        return Orientation::Vertical;
    if (x && z && y1 && a.acceleration.y < 0)
        return Orientation::VerticalInverse;
    if (y && z && x1 && a.acceleration.x > 0)
        return Orientation::Sideways;
    if (y && z && x1 && a.acceleration.x < 0)
        return Orientation::SidewaysInverse;
    
    return Orientation::None;
}

const char *orientationToString(Orientation o)
{
    switch (o)
    {
    case Orientation::Horizontal:
        return "Horizontal";
    case Orientation::HorizontalInverse:
        return "Horizontal Inverse";
    case Orientation::Vertical:
        return "Vertical";
    case Orientation::VerticalInverse:
        return "Vertical Inverse";
    case Orientation::Sideways:
        return "Sideways";
    case Orientation::SidewaysInverse:
        return "Sideways Inverse";
    case Orientation::None:
        return "None";
    }
    return "";
}

void gyro_setup()
{
    mpu.begin();
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}