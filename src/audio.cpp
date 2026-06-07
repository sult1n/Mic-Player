#include "audio.h"

c_Audio* audio;

std::string input_path(notcurses* nc, const char* prompt) {  // ai slop
    if (!nc) return "";
    ncplane* stdplane = notcurses_stdplane(nc);
    if (prompt) {
        ncplane_putstr(stdplane, prompt);
        notcurses_render(nc);
    }

    unsigned int y = 0, x = 0;
    ncplane_cursor_yx(stdplane, &y, &x);

    ncplane_options nopts = {};
    nopts.y = y;
    nopts.x = x;
    nopts.rows = 1;
    nopts.cols = ncplane_dim_x(stdplane) - x;
    ncplane* input_plane = ncplane_create(stdplane, &nopts);

    ncreader_options opts{};
    opts.flags = NCREADER_OPTION_CURSOR;
    ncreader* r = ncreader_create(input_plane, &opts);

    ncinput ni;
    while (true) {
        uint32_t key = notcurses_get(nc, nullptr, &ni);
        if (key == NCKEY_ENTER || key == '\n') break;
        if (key == NCKEY_ESC) {
            char* contents = nullptr;
            ncreader_destroy(r, &contents);
            if (contents) free(contents);
            return "";
        }

        ncreader_offer_input(r, &ni);
        notcurses_render(nc);
    }

    char* str = ncreader_contents(r);
    std::string result = str ? str : "";
    free(str);

    char* contents = nullptr;
    ncreader_destroy(r, &contents);
    if (contents) free(contents);

    return result;
}

void c_Player::play(audio_file* file) {
    audio->is_playing = true;

    ma_sound_start(&file->sound);
}

void c_Player::pause(audio_file* file) {
    if (audio->is_playing) {
        ma_sound_stop(&file->sound);
        audio->is_playing = false;
    } else {
        ma_sound_start(&file->sound);
        audio->is_playing = true;
    }
}

c_Player::c_Player(ma_engine* engine) {
    this->engine = engine;
}

c_Player::~c_Player() {
}

void c_Audio::send_command(commands command, audio_file* file) {
    this->command = command;
    this->file_to_play = file;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

void c_Audio::send_command(commands command) {
    this->command = command;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

audio_file* c_Audio::add_file(std::string& path_to_file) {
    auto file = std::make_unique<audio_file>();
    if (!std::filesystem::exists(path_to_file)) {
        std::cout << "there is no such file: " << path_to_file << std::endl;
        return nullptr;
    }

    ma_result res;
    if ((res = ma_sound_init_from_file(&this->engine, path_to_file.c_str(), 0, NULL, NULL, &file->sound)) != MA_SUCCESS) {
        std::cout << "error ma sound init" << std::endl;
        std::cout << "result is " << ma_result_description(res) << std::endl;
        return nullptr;
    }

    file->name = path_to_file;
    this->files.push_back(std::move(file));
    return this->files.back().get();
}

void c_Audio::update() {
    std::string path_to_file;
    std::cout << "enter path to file" << std::endl;
    render->pause_hotkeys_thread();
    path_to_file = input_path(render->nc, "path: ");
    render->resume_hotkeys_thread();
    auto file = this->add_file(path_to_file);
    if (!file) return;
    this->send_command(c_Audio::Play, file);

    while (true) {
        while (!this->is_update_running) {
            this->is_update_running.wait(false);
        }

        switch (this->command) {
            case c_Audio::Play:
                this->player->play(this->file_to_play);
            case c_Audio::Pause:
                this->player->pause(this->file_to_play);
        }

        this->is_update_running = false;
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

    if (!sizeof(this->playback.name)) {
        std::cout << "there is no virtual devices, use start.sh before mic player" << std::endl;
        return;
    }

    ma_engine_config engine_config = ma_engine_config_init();
    engine_config.pPlaybackDeviceID = &this->playback.id;
    if (ma_engine_init(&engine_config, &this->engine) != MA_SUCCESS) {
        std::cout << "error ma engine init" << std::endl;
        memset(&this->engine, 0, sizeof(this->engine));
        return;
    }

    this->player = std::make_unique<c_Player>(&this->engine);
}

c_Audio::~c_Audio() {
    ma_engine_uninit(&this->engine);
    ma_context_uninit(&this->context);
}