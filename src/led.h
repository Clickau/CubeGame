#pragma once
#include <Adafruit_NeoPixel.h>

const int pixelPin = 15;
Adafruit_NeoPixel pixels(72, pixelPin, NEO_GRB + NEO_KHZ800);

void led_setup()
{
    pixels.begin();
}

int map_leds(int actual_side)
{
    if (actual_side == 1) return 2;
    if (actual_side == 3) return 4;
    if (actual_side == 4) return 1;
    if (actual_side == 2) return 3;
    return actual_side;
}

// light up the twelve leds on one side with different colors
void led_display_side(int side, uint32_t colors[12])
{
    for (int i = 0; i < 12; i++)
    {
        colors[i] &= 0x3f3f3f;
    }
    
    Serial.printf("led_display_side: side %d, color %x\n", side, colors[0]);
    side = map_leds(side);
    for (int i = 0; i < 12; i++)
    {
        pixels.setPixelColor(12 * side + i, colors[i]);
    }
    pixels.show();
}