#include "audio.h"

c_Audio* audio;

void c_Player::play() {
    ma_sound sound;
    ma_result res;
    if ((res = ma_sound_init_from_file(&this->engine, "/home/eywan/asdfadsfa_fixed.wav", 0, NULL, NULL, &sound)) != MA_SUCCESS) {
        std::cout << "error ma sound init" << std::endl;
        std::cout << "result is " << ma_result_description(res) << std::endl;
    }

    ma_sound_start(&sound);

    c_Player::is_playing = true;
    while (ma_sound_is_playing(&sound)) {
        // ma_sound_stop(&sound);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ma_sound_uninit(&sound);
}

c_Player::c_Player(ma_device_info sink, std::string path_to_file) {
    ma_engine_config engine_config = ma_engine_config_init();
    engine_config.pPlaybackDeviceID = &sink.id;
    if (ma_engine_init(&engine_config, &this->engine) != MA_SUCCESS) {
        std::cout << "error ma engine init" << std::endl;
        return;
    }
}

c_Player::~c_Player() {
    ma_engine_uninit(&this->engine);
}

c_Player* c_Audio::create_player(std::string path_to_file) {
    if (!std::filesystem::exists(path_to_file)) {
        std::cout << "there is no such file: " << path_to_file << std::endl;
        return nullptr;
    }
    audio->players.emplace_back(std::make_unique<c_Player>(this->playback, path_to_file));

    return audio->players.back().get();
}

void c_Audio::update() {
    this->create_player("/home/eywan/asdfadsfa_fixed.wav");
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

c_Audio::c_Audio() {
    if (ma_context_init(0, 0, 0, &this->context) != MA_SUCCESS) {
        std::cout << "error ma context init" << std::endl;
        return;
    }

    ma_device_info* playback_infos;
    ma_uint32 playback_count;
    ma_device_info* capture_infos;
    ma_uint32 capture_count;
    if (ma_context_get_devices(&context, &playback_infos, &playback_count, &capture_infos, &capture_count) != MA_SUCCESS) {
        std::cout << "error ma context get devices" << std::endl;
        return;
    }

    for (int i = 0; i < playback_count; i++) {
        ma_device_info playback = playback_infos[i];
        if (strcmp(playback.name, "Virtual_Sink_for_Mic_Player") == 0) {
            this->playback = playback;
            break;
        }
    }
    if (!this->playback.name) {
        std::cout << "there is no virtual devices, use start.sh before mic player" << std::endl;
        return;
    }
}

c_Audio::~c_Audio() {
    ma_context_uninit(&this->context);
}