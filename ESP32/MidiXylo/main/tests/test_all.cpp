#include "m_xylo.hpp"
#include "adc_ctrl.hpp"
#include "mux.hpp"
#include "usb_midi.h"
#include "midi_msg.h"
#include "FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

int adc_to_midi(int val){
    float conv = val / 32.244;
    return static_cast<int>(conv);
}

extern "C" void app_main(void){
    int old = 0;
    MXylo mxylo;
    // AdcController adc_ctrl;
    // std::vector<adc_channel_t> channels{
    //     adc_channel_t(CONFIG_XMIDI_MUX_CHANNEL_0)
    // };
    // adc_ctrl.init_adc(channels);
    // sensors::mux_pin_config_t mux_pins = {
    //     .S0 = gpio_num_t(CONFIG_MUX0_PIN_S0),
    //     .S1 = gpio_num_t(CONFIG_MUX0_PIN_S1),
    //     .S2 = gpio_num_t(CONFIG_MUX0_PIN_S2),
    //     .S3 = gpio_num_t(CONFIG_MUX0_PIN_S3),
    // };
    // sensors::MuxController mux_ctrl(&mux_pins);
    // char* TAG = "XyloMidi test";
    // usb::UsbMidi usb_handle;
    // while(1){
    //     int val = adc_ctrl.read_adc(0);
    //     //ESP_LOGI(TAG, "Adc val: %d  ", val);
    //     int midi_val = adc_to_midi(val);
    //     //ESP_LOGI(TAG, "midi val: %d  ", midi);
    //     if(midi_val != old){
    //         // send midi CC msg
    //         midi::MidiPacket packet = {
    //             midi::msg::CC | 0,
    //             7,
    //             static_cast<uint8_t> (midi_val)
    //         };
    //         //ESP_LOGI(TAG, "send midi packet");
    //         usb_handle.send(midi::msg::cc(0, 7, midi_val));


    //         old = midi_val;
    //     }
    //     vTaskDelay(1);
    // }
}