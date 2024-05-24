#include <FS.h>
#include <SD.h>
#include <SPI.h>

// SD with MISO=19, MOSI=23, CLK=18, CS=5
const int sd_cs_pin = 5;

void sd_setup()
{
    SD.begin(sd_cs_pin);
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
    }
}

void sd_print_file(const char *path)
{
    File file = SD.open(path);
    if (file)
    {
        while (file.available())
        {
            Serial.write(file.read());
        }
        Serial.println();
        file.close();
    }
    else
    {
        Serial.println("error opening file");
    }
}