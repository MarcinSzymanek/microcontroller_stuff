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
    MXylo::instance().start();
    for(;;){
        vTaskDelay(1000);
    }
}