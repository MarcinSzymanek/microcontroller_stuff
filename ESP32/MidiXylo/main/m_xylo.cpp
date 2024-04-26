#include "m_xylo.hpp"
#include <vector>

MXylo::MXylo(){

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

}

void MXylo::start(){

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
