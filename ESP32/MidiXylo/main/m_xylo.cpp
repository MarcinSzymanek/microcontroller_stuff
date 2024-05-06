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

    display_.start_display();
}
QueueHandle_t MXylo::misc_int_queue;
QueueHandle_t MXylo::pad_int_queue;

void IRAM_ATTR pad_0_int_handler(void* arg){
    int i = (int)arg;
    xQueueSendFromISR(MXylo::pad_int_queue, &i, NULL);
}

void IRAM_ATTR misc_int_handler(void* arg){
    int i = (int)arg;
    xQueueSendFromISR(MXylo::misc_int_queue, &i, NULL);
}

void MXylo::task_scan_misc_(void* params){
    int interrupt_gpio;
    for(;;){
        if (xQueueReceive(misc_int_queue, &interrupt_gpio, portMAX_DELAY)){
            
            ESP_LOGI("task_scan_misc_", "Int gpio: %d", interrupt_gpio);
            if(interrupt_gpio == CONFIG_SWITCH_INT_PIN){
                MXylo::instance().scan_switches();
                continue;
            }
            
            int level = gpio_get_level((gpio_num_t) interrupt_gpio);
            if(!level) continue;
            ESP_LOGI("task_scan_misc_", "Scan buttons");
            MXylo::instance().scan_buttons();
        }
    }
}
//}

void MXylo::task_scan_pads_(void* params){
    int interrupt_gpio;
    for(;;){
        if (xQueueReceive(pad_int_queue, &interrupt_gpio, portMAX_DELAY)){
            //if(interrupt_gpio == CONFI)
            esp_err_t err = esp_timer_start_once(MXylo::instance().debug_timer, 500);
            //ESP_LOGI("Pads", "Interrupt received from int gpio: %d", interrupt_gpio);
            int level = gpio_get_level((gpio_num_t) interrupt_gpio);
            //ESP_LOGI("Pads", "Level: %d", gpio_get_level((gpio_num_t) interrupt_gpio));
            if(level){
                int mux_id = 0;
                if(interrupt_gpio == CONFIG_XMIDI_CHAN_MUX_PADS_1) mux_id = 1;
                // vTaskDelay(2/portTICK_PERIOD_MS); // Wait 1 ms before we measure

                int val = MXylo::instance().read_pads_(mux_id);
                //long time = esp_timer_get_time();
                esp_timer_dump(stderr);
                esp_timer_stop(MXylo::instance().debug_timer);
                //ESP_LOGI("Pads", "Measured: %d, time taken: %ld", val, time);
            }
        }
    }
}

void MXylo::scan_switches(){
    long time_0 = esp_timer_get_time();
    int buttons_state = read_switches_();
    ESP_LOGI("Switch scan", "buttons_state: %d", buttons_state);
    ESP_LOGI("Switch scan", "Sustain state: %d", sustain_val);
    if((buttons_state & 1) != static_cast<uint8_t>(mode_)){
        button_action_(MiscMappings::PROGRAM);
    }

    buttons_state = buttons_state >> 1;

    if((buttons_state & 1) != static_cast<uint8_t>(sustain_val)){
        button_action_(MiscMappings::SUSTAIN);
    }
    long time_1 = esp_timer_get_time();
    ESP_LOGI("Switch Scan:", "Time to read switches: %ld", time_1 - time_0);
}

void MXylo::scan_buttons(){
    int buttons_state = read_buttons_();
    // ESP_LOGI("Buttons", "Measured: %d, time taken: %ld", buttons_state, time_1 - time_0);
    long time_0 = esp_timer_get_time();
    for(int i = MUX_BUTTON_CHANNELS_[0]; i < MUX_BUTTON_CHANNELS_[1]; i++){
        if(buttons_state & 1){
            MXylo::instance().button_action_(static_cast<MiscMappings>(i));
        }
        buttons_state = buttons_state >> 1;
    }
    long time_1 = esp_timer_get_time();
    ESP_LOGI("Button Scan:", "Time to read buttons: %ld", time_1 - time_0);
}

void MXylo::start(){
    ESP_LOGI("MX", "Start");
    pad_int_queue = xQueueCreate(10, sizeof(int));
    misc_int_queue = xQueueCreate(5, sizeof(int));

    // Configure interrupts + create scan pads (0-15) task
    gpio_config_t int_config = {
        .pin_bit_mask = (1 << CONFIG_PAD_SET_0_INT_PIN) | (1 << CONFIG_SWITCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    gpio_config(&int_config);
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(
        (gpio_num_t) CONFIG_PAD_SET_0_INT_PIN,
        pad_0_int_handler,
        (void*) CONFIG_PAD_SET_0_INT_PIN
    );

    gpio_isr_handler_add(
        (gpio_num_t) CONFIG_SWITCH_INT_PIN,
        misc_int_handler,
        (void*) CONFIG_SWITCH_INT_PIN
    );

    xTaskCreate(&task_scan_pads_, "scan pads", 4096, NULL, (UBaseType_t) 2, NULL);
    xTaskCreate(&task_scan_misc_, "scan misc", 4096, NULL, (UBaseType_t) 3, NULL);
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

uint8_t MXylo::read_switches_(){
    uint8_t values = 0;
    vTaskDelay(1);
    for(int i = 0; i < 2; i++){
        mux_ctrls_[mux_type::MUX_MISC]->switch_channel(i);
        int val = adc_ctrl_->read_adc(AdcController::ADC_CHAN_MISC);
        if(val > 1000) values |= (1 << i);
    }
    return values;
}

uint8_t MXylo::read_buttons_(){
    uint8_t values = 0;
    vTaskDelay(1);
    for(int i = MUX_BUTTON_CHANNELS_[0]; i < MUX_BUTTON_CHANNELS_[1]; i++){
        mux_ctrls_[mux_type::MUX_MISC]->switch_channel(i);
        int val = adc_ctrl_->read_adc(AdcController::ADC_CHAN_MISC);
        if(val > 1000) values |= (1 << i);
    }
    return values;
}

// Decide what to do based on the button pressed
void MXylo::button_action_(MiscMappings&& button){
    ESP_LOGI("Button Action", "Button: %d", (int)button);

    if(button == MiscMappings::PROGRAM){
        mode_ = static_cast<Mode>((static_cast<int>(mode_) + 1) % (static_cast<int>(Mode::NUM_MODES)));
        Display::event_data_t data{static_cast<uint8_t>(mode_)};
        display_.push_event(Display::DISP_EVENT::MODE, std::move(data));
        return;
    }

    if(button == MiscMappings::SUSTAIN){
        if(sustain_val) sustain_val = 0;
        else sustain_val = 1;

        ESP_LOGI("Button action", "Sustain switch new val: %d", sustain_val);
        Display::event_data_t data{static_cast<uint8_t>(sustain_val)};
        display_.push_event(Display::DISP_EVENT::SUSTAIN, std::move(data));
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
            {
                if(octave_ == 3) return;
                octave_++;
                Display::event_data_t data{static_cast<uint8_t>(octave_ + 3), 0};
                display_.push_event(Display::DISP_EVENT::OCTAVE, std::move(data));
                break;
            }

        case(MiscMappings::OCTAVE_DOWN):
            {
                if(octave_ == -3) return;
                octave_--;
                Display::event_data_t data{static_cast<uint8_t>(octave_ + 3), 0};
                display_.push_event(Display::DISP_EVENT::OCTAVE, std::move(data));
                break;
            }

        case(MiscMappings::TRANSPOSE_UP):
            {
                if(transpose_ == 11) return;
                transpose_--;
                Display::event_data_t data{static_cast<uint8_t>(transpose_ + 11), 1};
                display_.push_event(Display::DISP_EVENT::TRANSPOSE, std::move(data));
                break;
            }

        case(MiscMappings::TRANSPOSE_DOWN):
            {
                if(transpose_ == -11) return;
                transpose_--;
                Display::event_data_t data{static_cast<uint8_t>(transpose_ + 11), 0};
                display_.push_event(Display::DISP_EVENT::TRANSPOSE, std::move(data));
                break;
            }

        case(MiscMappings::PROGRAM_CHANGE_UP):
            {
                if(patch_ == 126) return;
                patch_++;
                Display::event_data_t data{static_cast<uint8_t>(patch_), 1};
                display_.push_event(Display::DISP_EVENT::PROGRAM_CHANGE, std::move(data));
                usb_midi_.send(midi::msg::pc(channel_, patch_));
                break;
            }

        case(MiscMappings::PROGRAM_CHANGE_DOWN):
            {
                if(patch_ == 0) return;
                patch_--;
                Display::event_data_t data{static_cast<uint8_t>(patch_), 0};
                display_.push_event(Display::DISP_EVENT::PROGRAM_CHANGE, std::move(data));
                usb_midi_.send(midi::msg::pc(channel_, patch_));
                break;
            }

        default:
            return;
    }
}

void MXylo::program_change(bool up){
    if(up){
        patch_++;
        return;
    }

    patch_--;
}

void MXylo::program_button_action_(MiscMappings&& button){
    // Change channel
    if(button == MiscMappings::OCTAVE_UP || button == MiscMappings::TRANSPOSE_UP || button == MiscMappings::PROGRAM_CHANGE_UP){
        if(channel_ == 16) return;
        channel_++;
        display_.push_event(Display::DISP_EVENT::CHANNEL_CHANGE, Display::event_data_t{static_cast<uint8_t>(channel_ - 1), 1});
    }

    else if(button == MiscMappings::OCTAVE_DOWN || button == MiscMappings::TRANSPOSE_DOWN || button == MiscMappings::PROGRAM_CHANGE_DOWN){
        if(channel_ == 1) return;
        channel_--;
        display_.push_event(Display::DISP_EVENT::CHANNEL_CHANGE, Display::event_data_t{static_cast<uint8_t>(channel_ - 1), 0});

    }
}
