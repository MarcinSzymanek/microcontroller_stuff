#pragma once

#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include <vector>

class AdcController{
public:
    enum ADC_CHAN : uint8_t{
        ADC_CHAN_P0 = 0,
        ADC_CHAN_P1 = 1,
        ADC_CHAN_MISC = 2
    };

    enum ADC_TYPE{
        ADC_ONESHOT,
        ADC_CONTINUOUS
    };

    AdcController(){}
    ~AdcController();
    void init_adc(std::vector<adc_channel_t> &&channels);
    int read_adc();
    int read_adc(ADC_CHAN channel);
private:
    std::vector<adc_channel_t> channels_;
    adc_oneshot_unit_handle_t handle_;
    ADC_TYPE adc_type_ = ADC_TYPE::ADC_ONESHOT;
};