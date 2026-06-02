#include "device_wrapper.h"

std::vector<c_Sink*> c_Sink::sink_list;
std::vector<c_Mic*> c_Mic::mic_list;

c_Mic::c_Mic(ma_device_info info) {
    this->device_info = info;
    c_Mic::mic_list.push_back(this);
}

c_Sink::c_Sink(ma_device_info info) {
    this->device_info = info;
    c_Sink::sink_list.push_back(this);
}