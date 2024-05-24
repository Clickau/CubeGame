#include <Arduino.h>


#include "speaker.h"
#include "sd.h"
 
unsigned int t;

void setup() {
    Serial.begin(115200);
    
    sd_setup();
    speaker_setup();
    
    speaker_play_file("/MYMUSIC.mp3");
    t = millis();
}
 
void loop()
{
    // needs to be called regularly without big delays to send audio to speaker
    speaker_loop();
    // example how we can stop playing a song after 5 seconds
    if (millis() - t > 5 * 1000)
    {
        speaker_pause_resume();
        t = millis();
    }
}