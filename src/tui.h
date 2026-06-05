#pragma once
#include "includes.h"
#include "audio.h"

class c_TUI {
private:
    notcurses* nc;

public:
    std::atomic<bool> play_key_pressed;
    c_TUI();
    void render();
    void hotkeys();
};
