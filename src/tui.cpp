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

c_TUI::c_TUI() {
    std::thread thread(c_TUI::render);
    thread.detach();
}