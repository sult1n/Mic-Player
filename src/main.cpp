#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "includes.h"
#include "device_wrapper.h"
#include "audio.h"

int main()
{
    std::cout << "mic player v 0.1 is running" << std::endl;
    audio = new c_Audio;
    audio->init_player(c_Mic::mic_list[id]);
    return 0;
}
