#pragma once

#include "esp_log.h"

class Logger{
public:
    Logger(bool active)
    {
        ESP_LOGI("Hello", "world");
        active_ = active;
    }

    static void Log(const char* tag, const char* str){
        if(!active_) return;
        esp_log_write(ESP_LOG_INFO, tag, str);
    }
private:
    static bool active_;
};

bool Logger::active_ = false;