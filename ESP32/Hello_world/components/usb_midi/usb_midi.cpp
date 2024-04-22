#include <stdio.h>
#include "usb_midi.h"
#include "tinyusb.h"

namespace usb{
void func(void)
{
}

UsbMidi::UsbMidi(){
    std::array<const char*, 5> desc_ = {

    };

    tinyusb_config_t const tusb_cfg = {
        .device_descriptor = NULL, // If device_descriptor is NULL, tinyusb_driver_install() will use Kconfig
        .string_descriptor = s_str_desc,
        .string_descriptor_count = sizeof(s_str_desc) / sizeof(s_str_desc[0]),
        .external_phy = false,
#if (TUD_OPT_HIGH_SPEED)
        .fs_configuration_descriptor = s_midi_cfg_desc, // HID configuration descriptor for full-speed and high-speed are the same
        .hs_configuration_descriptor = s_midi_hs_cfg_desc,
        .qualifier_descriptor = NULL,
#else
        .configuration_descriptor = s_midi_cfg_desc,
#endif // TUD_OPT_HIGH_SPEED
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
}

constexpr size_t UsbMidi::get_total_desc_size(){
    return TUD_CONFIG_DESC_LEN + CFG_TUD_MIDI * TUD_MIDI_DESC_LEN;
}

void send(const MidiPacket &packet){
    tud_midi_packet_write(packet.data.data());
}

}
