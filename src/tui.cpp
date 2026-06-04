#include "tui.h"

void c_TUI::render() {
    while (true) {
        if (c_Player::is_playing) {
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
        std::lock_guard<std::mutex> lock(this->nc_mutex);

        ncinput ni;
        uint32_t key = notcurses_get_nblock(nc, &ni);
        switch (key) {
            case 0:
                std::cout << "hui" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
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