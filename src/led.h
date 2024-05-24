#include <Adafruit_NeoPixel.h>

const int pixelPin = 15;

Adafruit_NeoPixel pixels(12, pixelPin, NEO_GRB + NEO_KHZ800);

void led_setup()
{
    pixels.begin();
}

void led_display(uint32_t color)
{
    pixels.setPixelColor(0, color);
    pixels.setPixelColor(4, color);
    pixels.setPixelColor(8, color);
    pixels.show();
}