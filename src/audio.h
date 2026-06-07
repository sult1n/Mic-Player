#pragma once
#include "includes.h"
#include "tui.h"

struct audio_file {
    std::string name;
    ma_sound sound;

    ~audio_file() {
        ma_sound_uninit(&sound);
    }
};

class c_Player {
public:
    c_Player(ma_engine* engine);
    ~c_Player();
    void play(audio_file* file);
    void pause();

    ma_engine* engine;
    std::atomic<bool> is_playing;
};

class c_Audio {
public:
    enum commands : int {
        Play,
        Pause,
    };

    c_Audio();
    ~c_Audio();
    void update();
    void send_command(commands command, audio_file* file);
    void send_command(commands command);
    audio_file* add_file(std::string& path_to_file);

    std::unique_ptr<c_Player> player;
    std::vector<std::unique_ptr<audio_file>> files;
    audio_file* file_to_play;
    ma_context context;
    ma_device_info playback;
    ma_engine engine;
    std::atomic<bool> is_update_running;
    commands command;
};

extern c_Audio* audio;