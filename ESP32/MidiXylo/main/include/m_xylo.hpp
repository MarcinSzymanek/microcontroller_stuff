#pragma once

#include <memory>
#include <array>
#include "FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "adc_ctrl.hpp"
#include "display.hpp"
#include "mux.hpp"
#include "usb_midi.h"

#include "esp_timer.h"

class MXylo{
public:
    void start();
    static QueueHandle_t pad_int_queue;
    static QueueHandle_t misc_int_queue;
    static MXylo& instance(){
        static MXylo instance_;

        return instance_;
    }
    MXylo(MXylo const&) = delete;
    void operator=(MXylo const&) = delete;
private:
    MXylo();
    // Mappings of MUX_MISC
    enum MiscMappings : uint8_t{
        PROGRAM = 0,   // This will be a switch, but treat as a button in src
        SUSTAIN = 1,
        OCTAVE_UP = 2,
        OCTAVE_DOWN = 3,
        TRANSPOSE_UP = 4,
        TRANSPOSE_DOWN = 5,
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

    // indices of MISC_MUX's button channels
    // [0] : start
    // [1] : stop + 1
    // Default button channels are 2:7
    const std::array<const uint8_t, 2> MUX_BUTTON_CHANNELS_ = {
        2,
        8
    };
    const uint8_t MAX_KNOBS = 4;

    // Use 1 adc, 3 mux's
    std::shared_ptr<AdcController> adc_ctrl_;
    std::array<std::unique_ptr<sensors::MuxController>, 3> mux_ctrls_;

    Display display_;
    usb::UsbMidi usb_midi_;

    enum mux_type : uint8_t{
        MUX_P0 = 0,
        MUX_P1 = 1,
        MUX_MISC = 2
    };

    uint8_t read_pads_(int mux_id);
    uint8_t read_buttons_();
    uint8_t read_switches_();

    // Choose which action to perform based on button press
    void button_action_(MiscMappings&& button);
    void play_button_action_(MiscMappings&& button);
    void program_button_action_(MiscMappings&& button);

    // Actual actions to perform
    void toggle_mode();
    void toggle_sustain();
    void transpose_change(bool up);
    void octave_change(bool up);
    void program_change(bool up);
    void channel_change(bool up);

    void scan_buttons();
    void scan_switches();
    static void task_scan_pads_(void* params);
    static void task_scan_misc_(void* params);

    int timer_count = 0;
    esp_timer_cb_t debug_timer_cb;
    esp_timer_handle_t debug_timer;
    std::array<int, 1024> adc_buffer;
    size_t adc_buffer_pos = 0;
    uint8_t channel_{1};
    int8_t octave_{0};
    int8_t transpose_{0};
    int8_t patch_{0};
    bool sustain_on = false;
    uint8_t sustain_val = 0;

    int8_t last_scanned_misc;

};
