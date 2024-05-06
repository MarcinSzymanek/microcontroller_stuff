#include "display.hpp"
#include "open_lcd.hpp"

// Controls the M-xylo frontend (LCD)

// Wait for events and handle them
void task_process_events_(void* params){
    Display* display_ = static_cast<Display*>(params);
    Display::event_t received_event;
    Display::event_data_t event_data;
    for(;;){
        if(xQueueReceive(display_->event_queue, &received_event, 10)){
            event_data = received_event.data;
            switch(received_event.type){
                case(Display::DISP_EVENT::MODE):
                    display_->handle_mode_event(event_data[0]);
                    break;

                case(Display::DISP_EVENT::SUSTAIN):
                    display_->handle_sustain_event(event_data[0]); 
                    break;

                case(Display::DISP_EVENT::CHANNEL_CHANGE):
                    display_->handle_events(event_data[0], event_data[1], display_->channel_evt_vals);
                    break;

                case(Display::DISP_EVENT::PROGRAM_CHANGE):
                    display_->handle_events(event_data[0], event_data[1], display_->pc_evt_vals);
                    break;

                case(Display::DISP_EVENT::TRANSPOSE):
                    display_->handle_events(event_data[0], event_data[1], display_->transpose_evt_vals);
                    break;

                case(Display::DISP_EVENT::OCTAVE):
                    display_->handle_events(event_data[0], event_data[1], display_->octave_evt_vals);
                    break;
                default:
                    break;
            }
        }
    }
}

void Display::start_display(){
        lcd_.init_openLCD(CONFIG_LCD_I2C_SCL_PIN, CONFIG_LCD_I2C_SDA_PIN);
        for(int i = 0; i < 4; i++){
            lcd_.update_line(content_[i].c_str(), i);
        }
        xTaskCreate(task_process_events_, "display_events", 2048*3, this, 6, NULL);
}

void Display::push_event(DISP_EVENT&& event_type, event_data_t&& data){
    event_t event(event_type, data);
    xQueueSend(event_queue,  (void*) &event, 500);
}

void Display::handle_mode_event(uint8_t val){
    content_[3].replace(mode_pos, mode_[val].size(), mode_[val]);
    lcd_.update_line(content_[3].c_str(), 3);
}

void Display::handle_events(uint8_t val, uint8_t dir, event_vals_t event_const){
    std::string new_value = std::to_string(val);
    while(new_value.size() < event_const.max_val_len){
        new_value += ' ';
    }
    content_[event_const.line].replace(event_const.first_val_pos, event_const.max_val_len, new_value);
    std::string evt_string = event_const.evt_name;
    if(dir) evt_string += "++";
    else evt_string += "--";
    while(evt_string.size() < 14){
        evt_string += ' ';
    }
    content_[1].replace(evt_pos, 14, evt_string);
    lcd_.update_line(content_[1].c_str(), 1);
    lcd_.update_line(content_[event_const.line].c_str(), event_const.line);
}

void Display::handle_sustain_event(uint8_t val){
    std::string new_value;
    if(val) new_value = "ON ";
    else new_value = "OFF";
    content_[3].replace(sust_pos, 3, new_value);
    content_[1].replace(evt_pos, 11, "SUSTAIN " + new_value);
    vTaskDelay(2);
    lcd_.update_line(content_[1].c_str(), 1);
    vTaskDelay(2);
    lcd_.update_line(content_[3].c_str(), 3);
}

