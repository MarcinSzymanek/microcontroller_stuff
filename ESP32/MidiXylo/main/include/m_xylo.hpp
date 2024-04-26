#pragma once

#include <memory>
#include "adc_ctrl.hpp"
#include "mux.hpp"
#include <array>


class MXylo{
public:
    MXylo();
    void start();
private:
    // Mappings of MUX_MISC
    enum MiscMappings : uint8_t{
        PROGRAM = 0,   // This will be a switch, but treat as a button in src
        OCTAVE_UP = 1,
        OCTAVE_DOWN = 2,
        TRANSPOSE_UP = 3,
        TRANSPOSE_DOWN = 4,
        SUSTAIN = 5,
        PROGRAM_CHANGE_UP = 6,
        PROGRAM_CHANGE_DOWN = 7,

        // Knobs
        CC0 = 12,
        CC1 = 13,
        CC2 = 14,
        CC3 = 15,
        NONE = 200
    };
    enum class Mode{
        PLAY,
        PROGRAM,
        NUM_MODES
    };

    Mode mode_ = Mode::PLAY;

    const uint8_t MAX_BUTTONS = 8;
    const uint8_t MAX_KNOBS = 4;

    // Use 1 adc, 3 mux's
    std::shared_ptr<AdcController> adc_ctrl_;
    std::array<std::unique_ptr<sensors::MuxController>, 3> mux_ctrls_;

    enum mux_type : uint8_t{
        MUX_P0 = 0,
        MUX_P1 = 1,
        MUX_MISC = 2
    };

    uint8_t read_buttons_();
    void button_action_(MiscMappings&& button);
    void play_button_action_(MiscMappings&& button);
    void program_button_action_(MiscMappings&& button);

    uint8_t channel_{1};
    int8_t octave_{0};
    int8_t transpose_{0};
    bool sustain_on = false;

};