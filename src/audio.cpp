#include "audio.h"

c_Audio* audio;

std::atomic<bool> c_Player::is_playing;
c_Player::c_Player(ma_device_info sink) {
    std::cout << "c_Player ctr()" << std::endl;
    ma_engine_config engine_config = ma_engine_config_init();
    engine_config.pPlaybackDeviceID = &sink.id;
    if (ma_engine_init(&engine_config, &this->engine) != MA_SUCCESS) {
        std::cout << "error ma engine init" << std::endl;
    }

    ma_sound sound;
    ma_result res;
    if ((res = ma_sound_init_from_file(&this->engine, "/home/eywan/asdfadsfa_fixed.wav", 0, NULL, NULL, &sound)) != MA_SUCCESS) {
        std::cout << "error ma sound init" << std::endl;
        std::cout << "result is " << ma_result_description(res) << std::endl;
    }

    ma_sound_start(&sound);

    c_Player::is_playing = true;
    while (ma_sound_is_playing(&sound)) { }
    c_Player::is_playing = false;

    ma_sound_uninit(&sound);
}

c_Player::~c_Player() {
    ma_engine_uninit(&this->engine);
}

c_Audio::c_Audio() {
    if (ma_context_init(0, 0, 0, &this->context) != MA_SUCCESS) {
        std::cout << "error ma context init" << std::endl;
    }

    ma_device_info* playback_infos;
    ma_uint32 playback_count;
    ma_device_info* capture_infos;
    ma_uint32 capture_count;
    if (ma_context_get_devices(&context, &playback_infos, &playback_count, &capture_infos, &capture_count) != MA_SUCCESS) {
        std::cout << "error ma context get devices" << std::endl;
    }

    for (int i = 0; i < playback_count; i++) {
        auto playback = playback_infos[i];
        if (strcmp(playback.name, "Virtual_Sink_for_Mic_Player") == 0) {
            this->player = std::make_unique<c_Player>(playback);
            break;
        }
    }
    if (!this->player) {
        std::cout << "there is no virtual devices, use start.sh before mic player" << std::endl;
    }
}

c_Audio::~c_Audio() {
    ma_context_uninit(&this->context);
}