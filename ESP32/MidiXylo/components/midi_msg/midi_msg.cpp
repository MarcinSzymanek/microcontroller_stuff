#include "midi_msg.h"
#include "stdio.h"

namespace midi{

    namespace msg{

        //template<uint8_t value>
        //concept Channel

        MidiPacket noteOn(uint8_t channel, uint8_t note_value, uint8_t velocity){
            // assert(channel >= 0 && channel < 16);
            // assert(note_value >= 0 && note_value < 127);
            // assert(velocity >= 0 && velocity < 127);
            printf("Creating midi packet");
            return MidiPacket{ static_cast<uint8_t>(MIDI_STATUS::NOTE_ON | channel), note_value, velocity};
        }
    } // namespace msg

}   // namespace midi


