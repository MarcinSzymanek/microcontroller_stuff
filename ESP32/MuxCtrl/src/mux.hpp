#include "stdint.h"
#include "esp_log.h"

typedef struct mux_pin_config_t{
    gpio_num_t S0;
    gpio_num_t S1;
    gpio_num_t S2;
    gpio_num_t S3;
}mux_pin_config;

class MuxController{
public:
    MuxController(){}
    MuxController(mux_pin_config* mux_pins){
        init_mux(mux_pins);
    }
    void init_mux(mux_pin_config* mux_pins);
    void switch_channel(uint8_t channel);
private:
    mux_pin_config mux_pins_;
    static int MUX_ID;
    int mux_id_;
};

int MuxController::MUX_ID = 0;