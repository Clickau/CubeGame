// #include <driver/i2s.h>
#include <Audio.h>

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;

void speaker_setup()
{
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

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21);
    // audio.connecttoFS(SD, "/sound.wav");
}

void speaker_loop()
{
    audio.loop();
}