#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "includes.h"
#include "audio.h"
#include "tui.h"

int main()
{
    std::cout << "mic player v 0.1 is running" << std::endl;
    audio = new c_Audio;
    render = new c_TUI;
    audio->update();
    notcurses_stop(render->nc);
    system("stty sane");
    std::cerr << "\033[?25h\033[0m" << std::flush;
    return 0;
}
