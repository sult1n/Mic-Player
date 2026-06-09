#include "tui.h"

c_TUI* render;

void c_TUI::render() {
    while (true) {
        if (audio->is_playing) {
            std::cout << "----------------------" << std::endl;
            std::cout << "current volume is " << audio->volume << std::endl;
            std::cout << "playing " << audio->player->get_cursor(audio->file_to_play) << "/" << audio->player->get_length(audio->file_to_play) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "\033[3A\033[0J" << std::flush;
        }
    }
}

void c_TUI::pause_hotkeys_thread() {
    this->hotkey_thread_paused = true;
}

void c_TUI::resume_hotkeys_thread() {
    this->hotkey_thread_paused = false;
    this->hotkey_thread_paused.notify_one();
}

void c_TUI::hotkeys() {
    while (true) {
        while (this->hotkey_thread_paused) {
            this->hotkey_thread_paused.wait(true);
        }
        ncinput ni;
        uint32_t key = notcurses_get(nc, nullptr, &ni);
        if (!key || ni.evtype != NCTYPE_PRESS) continue;
        switch (key) {
            case 'p':
                audio->send_command(c_Audio::commands::Pause);
                break;
            case '=':
                audio->send_command(c_Audio::commands::ChangeVolume, audio->volume += 0.1);
                break;
            case '-':
                if (audio->volume > 0)
                    audio->send_command(c_Audio::commands::ChangeVolume, audio->volume -= 0.1);
                break;
            case NCKEY_RIGHT:
                audio->send_command(c_Audio::commands::Seek, 1, false);
                break;
            case NCKEY_LEFT:
                audio->send_command(c_Audio::commands::Seek, 1, true);
                break;
        }
    }
}

c_TUI::c_TUI() {
    this->nc = notcurses_init(0, 0);
    if (!this->nc) {
        std::cout << "error notcurses init" << std::endl;
        return;
    }

    std::thread render_thread(&c_TUI::render, this);
    std::thread hotkeys_thread(&c_TUI::hotkeys, this);
    render_thread.detach();
    hotkeys_thread.detach();
}

c_TUI::~c_TUI() {
}