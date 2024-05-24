// #include <driver/i2s.h>
#include <Audio.h>

const int speaker_pin_dout = 22;
const int speaker_pin_bclk = 26;
const int speaker_pin_lrc = 25;

Audio audio;

void speaker_setup()
{
    audio.setPinout(speaker_pin_bclk, speaker_pin_lrc, speaker_pin_dout);
    audio.setVolume(21);
}

void speaker_play_file(const char *path)
{
    audio.connecttoFS(SD, path);
}

void speaker_pause_resume()
{
    audio.pauseResume();
}

void speaker_stop_playback()
{
    audio.stopSong();
}

void speaker_loop()
{
    audio.loop();
}