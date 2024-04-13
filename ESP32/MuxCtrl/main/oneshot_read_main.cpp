/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/gpio.h"
#include "mux.cpp"
#include "adc_ctrl.cpp"

const static char *TAG = "EXAMPLE";

#define MUX_GPIO_S0 GPIO_NUM_15
#define MUX_GPIO_S1 GPIO_NUM_2
#define MUX_GPIO_S2 GPIO_NUM_4
#define MUX_GPIO_S3 GPIO_NUM_16

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
//ADC1 Channels
#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_4
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_5
#else
#define EXAMPLE_ADC1_CHAN0          ADC_CHANNEL_2
#define EXAMPLE_ADC1_CHAN1          ADC_CHANNEL_3
#endif

#if (SOC_ADC_PERIPH_NUM >= 2) && !CONFIG_IDF_TARGET_ESP32C3
/**
 * On ESP32C3, ADC2 is no longer supported, due to its HW limitation.
 * Search for errata on espressif website for more details.
 */
#define EXAMPLE_USE_ADC2            0
#endif

#if EXAMPLE_USE_ADC2
//ADC2 Channels
#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_ADC2_CHAN0          ADC_CHANNEL_0
#else
#define EXAMPLE_ADC2_CHAN0          ADC_CHANNEL_0
#endif
#endif  //#if EXAMPLE_USE_ADC2

#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12

static int adc_raw[2][10];
static int voltage[2][10];
static bool example_adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void example_adc_calibration_deinit(adc_cali_handle_t handle);

extern "C" void app_main(void)
{
    MuxController mux_ctrl;
    mux_pin_config cfg = {
        MUX_GPIO_S0,
        MUX_GPIO_S1,
        MUX_GPIO_S2,
        MUX_GPIO_S3
    };
    mux_ctrl.init_mux(&cfg);

    //-------------ADC1 Init---------------//

    AdcController adc;

    std::vector<adc_channel_t> channels {ADC_CHANNEL_4};
    adc.init_adc(channels);

    uint8_t mux_channel = 0;
    while (1) {

        mux_ctrl.switch_channel(mux_channel);
        int data = adc.read_adc();
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1, ADC_CHANNEL_4, data);
        ESP_LOGI(TAG, "Mux switch %d", mux_channel);
        if(mux_channel == 0) mux_channel = 2;
        else mux_channel = 0;
        vTaskDelay(pdMS_TO_TICKS(2000));

    }

    //Tear Down

}



