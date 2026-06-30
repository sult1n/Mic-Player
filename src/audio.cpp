#include "audio.h"
#include "tui.h"

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
    nopts.y = static_cast<int>(y);
    nopts.x = static_cast<int>(x);
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
    if (audio->is_playing) {
        ma_sound_stop(&file->sound);
        audio->is_playing = false;
    } else {
        ma_sound_start(&file->sound);
        audio->is_playing = true;
    }
}

void c_Player::change_volume(audio_file* file, float volume) {
    ma_sound_set_volume(&file->sound, volume);
}

void c_Player::seek(audio_file* file, float second) {
    auto sample_rate = ma_engine_get_sample_rate(this->engine);
    auto target_frame = second * static_cast<float>(sample_rate);
    ma_sound_seek_to_pcm_frame(&file->sound, static_cast<ma_uint64>(target_frame));
}

float c_Player::get_cursor(audio_file* file) {
    ma_uint64 cursor_in_frames = 0;
    ma_uint32 sample_rate = 0;
    ma_sound_get_cursor_in_pcm_frames(&file->sound, &cursor_in_frames);
    ma_sound_get_data_format(&file->sound, 0, 0, &sample_rate, 0, 0);
    float current_second = static_cast<float>(cursor_in_frames) / static_cast<float>(sample_rate);
    return current_second;
}

float c_Player::get_length(audio_file* file) {
    ma_uint64 length_in_frames = 0;
    ma_uint32 sample_rate = 0;

    ma_sound_get_length_in_pcm_frames(&file->sound, &length_in_frames);
    ma_sound_get_data_format(&file->sound, nullptr, nullptr, &sample_rate, nullptr, 0);

    float total_seconds = static_cast<float>(length_in_frames) / static_cast<float>(sample_rate);
    return total_seconds;
}

c_Player::c_Player(ma_engine* engine_) {
    this->engine = engine_;
}

void c_Audio::send_command(commands command_, audio_file* file) {
    this->command = command_;
    this->file_to_play = file;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

void c_Audio::send_command(commands command_, float volume_, bool is_mute) {
    this->command = command_;
    if (!is_mute)
        this->volume = volume_;
    else
        this->is_muted = !this->is_muted;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

void c_Audio::send_command(commands command_, float second) {
    this->command = command_;
    this->second_to_seek = this->player->get_cursor(this->file_to_play) + second;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

void c_Audio::send_command(commands command_) {
    this->command = command_;
    this->is_update_running = true;
    this->is_update_running.notify_one();
}

audio_file* c_Audio::add_file(std::string& path_to_file) {
    auto file = std::make_unique<audio_file>();
    if (!std::filesystem::exists(path_to_file)) {
        std::cout << "there is no such file: " << path_to_file << std::endl;
        return nullptr;
    }

    ma_result res  = ma_sound_init_from_file(&this->engine, path_to_file.c_str(), 0, 0, 0, &file->sound);
    if (res != MA_SUCCESS) {
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
            case c_Audio::commands::Play:
                this->player->play(this->file_to_play);
                break;
            case c_Audio::commands::ChangeVolume:
                if (this->is_muted)
                    this->player->change_volume(this->file_to_play, 0);
                else
                    this->player->change_volume(this->file_to_play, volume);
                break;
            case c_Audio::commands::Seek:
                this->player->seek(this->file_to_play, this->second_to_seek);
                break;
            default:
                break;
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

    for (ma_uint32 i = 0; i < playback_count; i++) {
        ma_device_info playback_ = playback_infos[i];
        if (strcmp(playback.name, "Virtual_Sink_for_Mic_Player") == 0) {
            this->playback = playback_;
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
