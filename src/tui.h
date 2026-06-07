#pragma once
#include "includes.h"
#include "audio.h"

class c_TUI {
public:
    c_TUI();
    ~c_TUI();
    void render();
    void pause_hotkeys_thread();
    void resume_hotkeys_thread();
    void hotkeys();

    notcurses* nc;
    std::atomic<bool> play_key_pressed;
    std::atomic<bool> hotkey_thread_paused;
};

extern c_TUI* render;
