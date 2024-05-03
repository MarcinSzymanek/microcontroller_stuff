#include "m_xylo.hpp"
#include "esp_timer.h"
#include <vector>
int timer_count = 0;
static void debug_timer_cb(void*){
    timer_count++;
}

MXylo::MXylo(){
    ESP_LOGI("MX", "M-xylo init");
    std::vector<adc_channel_t> channels = {
        adc_channel_t(CONFIG_XMIDI_CHAN_MUX_PADS_0),
        adc_channel_t(CONFIG_XMIDI_CHAN_MUX_PADS_1),
        adc_channel_t(CONFIG_XMIDI_CHAN_MUX_MISC)
    };

    adc_ctrl_ = std::make_shared<AdcController>();
    adc_ctrl_->init_adc(std::move(channels));

    sensors::mux_pin_config mux_p0_pins = {
        .S0 = gpio_num_t(CONFIG_MUX_P0_PIN_S0),
        .S1 = gpio_num_t(CONFIG_MUX_P0_PIN_S1),
        .S2 = gpio_num_t(CONFIG_MUX_P0_PIN_S2),
        .S3 = gpio_num_t(CONFIG_MUX_P0_PIN_S3),
    };
    sensors::mux_pin_config mux_p1_pins = {
        .S0 = gpio_num_t(CONFIG_MUX_P1_PIN_S0),
        .S1 = gpio_num_t(CONFIG_MUX_P1_PIN_S1),
        .S2 = gpio_num_t(CONFIG_MUX_P1_PIN_S2),
        .S3 = gpio_num_t(CONFIG_MUX_P1_PIN_S3),
    };
    sensors::mux_pin_config mux_misc_pins = {
        .S0 = gpio_num_t(CONFIG_MUX_MISC_PIN_S0),
        .S1 = gpio_num_t(CONFIG_MUX_MISC_PIN_S1),
        .S2 = gpio_num_t(CONFIG_MUX_MISC_PIN_S2),
        .S3 = gpio_num_t(CONFIG_MUX_MISC_PIN_S3),
    };

    mux_ctrls_[0] = std::make_unique<sensors::MuxController>(&mux_p0_pins);
    mux_ctrls_[1] = std::make_unique<sensors::MuxController>(&mux_p1_pins);
    mux_ctrls_[2] = std::make_unique<sensors::MuxController>(&mux_misc_pins);

    
    esp_timer_create_args_t args = {
        .callback = debug_timer_cb,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "one-shot"
    };
    esp_timer_create(&args, &debug_timer);
}
QueueHandle_t MXylo::pad_int_queue;
void IRAM_ATTR pad_0_int_handler(void* arg){
    int i = (int)arg;
    xQueueSendFromISR(MXylo::pad_int_queue, &i, NULL);
}

void MXylo::task_scan_pads_(void* params){
    int interrupt_gpio;
    for(;;){
        if (xQueueReceive(pad_int_queue, &interrupt_gpio, portMAX_DELAY)){
            esp_err_t err = esp_timer_start_once(MXylo::instance().debug_timer, 500);
            esp_timer_dump(stderr);
            //ESP_LOGI("Pads", "Interrupt received from int gpio: %d", interrupt_gpio);
            int level = gpio_get_level((gpio_num_t) interrupt_gpio);
            //ESP_LOGI("Pads", "Level: %d", gpio_get_level((gpio_num_t) interrupt_gpio));
            if(level){

                int mux_id = 0;
                if(interrupt_gpio == CONFIG_XMIDI_CHAN_MUX_PADS_1) mux_id = 1;
                // vTaskDelay(2/portTICK_PERIOD_MS); // Wait 1 ms before we measure

                int val = MXylo::instance().read_pads_(mux_id);
                long time = esp_timer_get_time();
                esp_timer_dump(stderr);
                esp_timer_stop(MXylo::instance().debug_timer);
                ESP_LOGI("Pads", "Measured: %d, time taken: %ld", val, time);
            }
        }
    }
}

void MXylo::start(){
    ESP_LOGI("MX", "Start");
    mux_ctrls_[0]->switch_channel(0);
    mux_ctrls_[1]->switch_channel(0);
    mux_ctrls_[2]->switch_channel(0);
    pad_int_queue = xQueueCreate(10, sizeof(int));

    // Configure interrupts
    gpio_config_t int_config = {
        .pin_bit_mask = (1 << CONFIG_PAD_SET_0_INT_PIN) | (1 << CONFIG_SWITCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    gpio_config(&int_config);
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(
        (gpio_num_t) CONFIG_PAD_SET_0_INT_PIN,
        pad_0_int_handler,
        (void*) CONFIG_PAD_SET_0_INT_PIN
    );

    mux_ctrls_[0]->switch_channel(0);
    mux_ctrls_[1]->switch_channel(0);
    mux_ctrls_[2]->switch_channel(0);
    xTaskCreate(&task_scan_pads_, "scan pads", 2048, NULL, (UBaseType_t) 2, NULL);
    
}

uint8_t MXylo::read_pads_(int mux_id){
    uint8_t end = 16;
    ESP_LOGI("ReadPads", "Mux id: %d", mux_id);
    if(mux_id > 0) end = 9; 
    int high = 0;
    for(int i = 0; i < end; i++){
        mux_ctrls_[mux_id]->switch_channel(i);
        int val = adc_ctrl_->read_adc();
        if(val > high) high = i;
        adc_buffer[adc_buffer_pos] = val;
        adc_buffer_pos++;
        if(adc_buffer_pos > adc_buffer.max_size());
    }
    return high;
}

uint8_t MXylo::read_buttons_(){
    for(int i = 0; i < MAX_BUTTONS; i++){
        mux_ctrls_[mux_type::MUX_MISC]->switch_channel(i);
        int val = adc_ctrl_->read_adc(CONFIG_XMIDI_CHAN_MUX_MISC);
        if(val) return i;
    }

    return MiscMappings::NONE;
}

// Decide what to do based on the button pressed
void MXylo::button_action_(MiscMappings&& button){
    if(button == MiscMappings::NONE) return;

    if(button == MiscMappings::PROGRAM){
        mode_ = static_cast<Mode>((static_cast<int>(mode_) + 1) % (static_cast<int>(Mode::NUM_MODES)));
        return;
    }

    if(mode_ == Mode::PLAY){
        play_button_action_(std::move(button));
        return;
    }

    program_button_action_(std::move(button));
}

void MXylo::play_button_action_(MiscMappings&& button){
    switch(button){

        case(MiscMappings::OCTAVE_UP):
            if(octave_ == 3) return;
            octave_++;
            break;

        case(MiscMappings::OCTAVE_DOWN):
            if(octave_ == -3) return;
            octave_--;
            break;

        case(MiscMappings::TRANSPOSE_UP):
            if(transpose_ == 11) return;
            transpose_++;
            break;

        case(MiscMappings::TRANSPOSE_DOWN):
            if(transpose_ == -11) return;
            transpose_--;
            break;

        case(MiscMappings::SUSTAIN):
            sustain_on = !sustain_on;
            break;

        default:
            return;
    }
}

void MXylo::program_button_action_(MiscMappings&& button){
    // Change channel
    if(button == MiscMappings::OCTAVE_UP || button == MiscMappings::TRANSPOSE_UP){
        channel_++;
        if(channel_ > 16) channel_ = 1;
    }

    else if(button == MiscMappings::OCTAVE_DOWN || button == MiscMappings::TRANSPOSE_DOWN){
        channel_--;
        if(channel_ < 1) channel_ = 16;
    }
}
