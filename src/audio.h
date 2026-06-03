#pragma once
#include "includes.h"

class c_Player {
public:
    c_Player(ma_device_info sink);
    ~c_Player();

    ma_engine engine;
    static std::atomic<bool> is_playing;
};

class c_Audio {
public:
    c_Audio();
    ~c_Audio();

    std::unique_ptr<c_Player> player = nullptr;
    ma_context context;
};

extern c_Audio* audio;