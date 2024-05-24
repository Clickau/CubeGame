#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
// #include <driver/i2s.h>
#include <Audio.h>

const int pixelPin = 15;
// MPU with scl=22, sda=21
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26


Adafruit_MPU6050 mpu;
Adafruit_NeoPixel pixels(12, pixelPin, NEO_GRB + NEO_KHZ800);
Audio audio;

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

Orientation state;

void display(uint32_t c)
{
    pixels.setPixelColor(0, c);
    pixels.setPixelColor(4, c);
    pixels.setPixelColor(8, c);
    pixels.show();
}

void mpu_setup()
{
    mpu.begin();
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void readFile(const char *path)
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

// void setup_speaker()
// {
//     i2s_config_t i2s_config = {
//         .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
//         .sample_rate = 44100,
//         .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//         .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//         .communication_format = I2S_COMM_FORMAT_I2S_MSB,
//         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//         .dma_buf_count = 8,
//         .dma_buf_len = 1024,
//         .use_apll = false,
//         .tx_desc_auto_clear = true,
//     };

//     i2s_pin_config_t pin_config = {
//         .bck_io_num = I2S_BCLK,
//         .ws_io_num = I2S_LRC,
//         .data_out_num = I2S_DOUT,
//         .data_in_num = -1,
//     };
//     i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//     i2s_set_pin(I2S_NUM_0, &pin_config);
//     i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
//     i2s_set_sample_rates(I2S_NUM_0, 44100);
// }

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(10);

    mpu_setup();

    Serial.println("test");
    pixels.begin();
    display(0x010000);

    state = Orientation::None;

    SD.begin(5);
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    
    // setup_speaker();

    readFile("/test.txt");

    // File file = SD.open("/sound.wav");
    // if (!file)
    // {
    //     Serial.println("!sound");
    //     return;
    // }

    // const size_t buffer_size = 512;
    // uint8_t buffer[buffer_size];
    // size_t bytes_read;
    // size_t bytes_written;
    // while (file.available() && (bytes_read = file.read(buffer, buffer_size)) > 0) {
    //     i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
    //     if (bytes_read == 0) break;
    // }
    // file.close();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21);
    audio.connecttoFS(SD, "/sound.wav");
}

void loop()
{
    audio.loop();
    // uint32_t t = audio.getAudioCurrentTime();
    // Serial.println(t);
    // Orientation o = getOrientation();
    // if (o != state && o != Orientation::None)
    // {
    //     state = o;
    //     Serial.println(orientationToString(state));
    // }
    // delay(500);
}
