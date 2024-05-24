#include <Arduino.h>

#include "gyro.h"
#include "led.h"
#include "sd.h"
#include "speaker.h"

int orientation = -1;

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(10);

    gyro_setup();
    led_setup();
    sd_setup();
    speaker_setup();
}

void loop()
{
    int o = gyro_get_orientation();
    if (o != orientation && o != -1)
    {
        orientation = o;
        Serial.println(o);
    }
    delay(500);
}
