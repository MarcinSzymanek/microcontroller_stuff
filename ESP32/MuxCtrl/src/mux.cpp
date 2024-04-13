#include "mux.hpp"
#include "driver/gpio.h"
#include "esp_log.h"



esp_err_t conf_gpio(gpio_num_t gpio){
        esp_err_t err;
        err = gpio_reset_pin(gpio);
        if(err != ESP_OK) return err;
        err = gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
        return err;
    }

void MuxController::init_mux(mux_pin_config* mux_pins){
    mux_pins_.S0 = mux_pins->S0;
    mux_pins_.S1 = mux_pins->S1;
    mux_pins_.S2 = mux_pins->S2;
    mux_pins_.S3 = mux_pins->S3;
    ESP_ERROR_CHECK(conf_gpio(mux_pins_.S0));
    ESP_ERROR_CHECK(conf_gpio(mux_pins_.S1));
    ESP_ERROR_CHECK(conf_gpio(mux_pins_.S2));
    ESP_ERROR_CHECK(conf_gpio(mux_pins_.S3));
    mux_id_ = MUX_ID;
    MUX_ID++;
}

void MuxController::switch_channel(uint8_t channel){
    gpio_set_level(mux_pins_.S0, channel & 1);
    gpio_set_level(mux_pins_.S1, (channel >> 1) & 1);
    gpio_set_level(mux_pins_.S2, (channel >> 2) & 1);
    gpio_set_level(mux_pins_.S3, (channel >> 3) & 1);

}
