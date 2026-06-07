#pragma once
#include "includes.h"
#include "tui.h"

class c_Player {
public:
    c_Player(ma_device_info sink, std::string path_to_file);
    ~c_Player();
    void play();

    ma_engine engine;
    std::string path_to_file;
    bool is_playing;
};

class c_Audio {
public:
    c_Audio();
    ~c_Audio();
    void update();
    c_Player* create_player(std::string path_to_file);

    std::vector<std::unique_ptr<c_Player>> players;
    ma_context context;
    ma_device_info playback;
    std::atomic<uint> current_player;
    std::atomic<bool> is_any_playing_player;
};

extern c_Audio* audio;