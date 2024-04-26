#pragma once

#include "stdint.h"
#include "esp_log.h"
#include "driver/gpio.h"

namespace sensors{

typedef struct mux_pin_config_t{
    gpio_num_t S0;
    gpio_num_t S1;
    gpio_num_t S2;
    gpio_num_t S3;
} mux_pin_config;

class MuxController{
    public:
        MuxController(){}
        MuxController(mux_pin_config* mux_pins);
        void init_mux(mux_pin_config* mux_pins);
        void switch_channel(uint8_t channel);
    private:
        mux_pin_config mux_pins_;
        static int MUX_ID;
        int mux_id_;
    };

} // namespace sensors

