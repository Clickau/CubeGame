#include <Arduino.h>

#include "gyro.h"
#include "led.h"
#include "sd.h"
#include "speaker.h"

Orientation state;

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(10);

    gyro_setup();
    led_setup();
    sd_setup();
    speaker_setup();

    state = Orientation::None;
}

void loop()
{
    Orientation o = getOrientation();
    if (o != state && o != Orientation::None)
    {
        state = o;
        Serial.println(orientationToString(state));
    }
    delay(500);
}
