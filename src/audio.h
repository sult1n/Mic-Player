#pragma once
#include "includes.h"

struct audio_file {
    std::string name;
    ma_sound sound;

    ~audio_file() {
        ma_sound_uninit(&sound);
    }
};

class c_Player {
public:
    c_Player(ma_engine* engine_);
    void play(audio_file* file);
    void pause(audio_file* file);
    void change_volume(audio_file* file, float volume);
    void seek(audio_file* file, float second);
    float get_cursor(audio_file* file);
    float get_length(audio_file* file);
    ma_engine* engine;
};

class c_Audio {
public:
    enum commands : uint8_t {
        Play,
        ChangeVolume,
        Seek,
    };

    c_Audio();
    ~c_Audio();
    void update();
    void send_command(commands command_, audio_file* file);
    void send_command(commands command_, float volume, bool is_mute);
    void send_command(commands command_, float second);
    void send_command(commands command_);
    audio_file* add_file(std::string& path_to_file);

    std::unique_ptr<c_Player> player;
    std::vector<std::unique_ptr<audio_file>> files;
    audio_file* file_to_play;
    ma_context context;
    ma_device_info playback;
    ma_engine engine;
    std::atomic<float> volume;
    std::atomic<float> second_to_seek;
    std::atomic<bool> is_playing;
    std::atomic<bool> is_muted;
    std::atomic<bool> is_update_running;
    std::atomic<commands> command;
};

extern c_Audio* audio;
