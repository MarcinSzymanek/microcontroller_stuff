#include <stdio.h>
#include "usb_midi.h"
#include "tinyusb.h"

namespace usb{
void func(void)
{
}

constexpr size_t UsbMidi::get_total_desc_size(){
    return TUD_CONFIG_DESC_LEN + CFG_TUD_MIDI * TUD_MIDI_DESC_LEN;
}

void send(const MidiPacket &packet){
    tud_midi_packet_write(packet.data.data());
}

}
