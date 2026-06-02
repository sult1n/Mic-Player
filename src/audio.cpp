#include "audio.h"

c_Audio* audio;

c_Player::c_Player(c_Sink* sink) {
    ma_engine_config engine_config = ma_engine_config_init();
    engine_config.pPlaybackDeviceID = &sink->device_info.id;
    if (ma_engine_init(&engine_config, &this->engine) != MA_SUCCESS) {
        std::cout << "error ma engine init" << std::endl;
    }
}

c_Audio::c_Audio() {
    if (ma_context_init(0, 0, 0, &this->context) != MA_SUCCESS) {
        std::cout << "error ma context init" << std::endl;
    }
}

c_Audio::~c_Audio() {
    ma_context_uninit(&this->context);
}