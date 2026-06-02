#pragma once
#include "includes.h"
#include "device_wrapper.h"

class c_Player {
public:
    c_Player(c_Sink* sink);

    ma_engine engine;
};

class c_Audio {
public:
    c_Audio();
    ~c_Audio();

    std::unique_ptr<c_Player> player;
    ma_context context;
};

extern c_Audio* audio;