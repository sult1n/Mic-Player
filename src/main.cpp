#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "includes.h"
#include "audio.h"
#include "tui.h"

int main()
{
    std::cout << "mic player v 0.1 is running" << std::endl;
    // auto render = new c_TUI;
    audio = new c_Audio;
    audio->update();
    return 0;
}
