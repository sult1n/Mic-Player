#include "audio.h"

c_Audio* audio;

c_Player::c_Player(ma_device_info sink) {
    std::cout << "c_Player ctr()" << std::endl;
    ma_engine_config engine_config = ma_engine_config_init();
    engine_config.pPlaybackDeviceID = &sink.id;
    if (ma_engine_init(&engine_config, &this->engine) != MA_SUCCESS) {
        std::cout << "error ma engine init" << std::endl;
    }
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
        if (strcmp(playback.name, "Virtual_Sink_for_Mic_Player")) {
            this->player = std::make_unique<c_Player>(playback);
            break;
        }
    }
}

c_Audio::~c_Audio() {
    ma_context_uninit(&this->context);
}