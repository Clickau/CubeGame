#include <Adafruit_NeoPixel.h>

const int pixelPin = 15;
Adafruit_NeoPixel pixels(72, pixelPin, NEO_GRB + NEO_KHZ800);

void led_setup()
{
    pixels.begin();
}

// light up the twelve leds on one side with different colors
void led_display_side(int side, uint32_t colors[12])
{
    for (int i = 0; i < 12; i++)
    {
        pixels.setPixelColor(12 * side + i, colors[i]);
    }
    pixels.show();
}