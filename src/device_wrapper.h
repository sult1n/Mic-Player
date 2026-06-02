#pragma once
#include "includes.h"

class c_Sink {
public:
    c_Sink(ma_device_info);

    static std::vector<c_Sink*> sink_list;
    ma_device_info device_info;
};

class c_Mic {
public:
    c_Mic(ma_device_info info);

    static std::vector<c_Mic*> mic_list;
    ma_device_info device_info;
};