#include "tui.h"

void c_TUI::render() {
    while (true) {
        if (audio->is_any_playing_player) {
            std::cout << "----------------------" << std::endl;
            std::cout << "sound is playing" << std::endl;
            std::cout << "----------------------" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "\033[3A\033[0J" << std::flush;
        }
    }
}

void c_TUI::hotkeys() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(300));
        std::lock_guard<std::mutex> lock(this->nc_mutex);

        ncinput ni;
        uint32_t key = notcurses_get_nblock(nc, &ni);
        if (!key) continue;
        switch (key) {
            case 'p':
                this->play_key_pressed = !this->play_key_pressed;
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