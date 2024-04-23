#pragma once

#include <initializer_list>
#include <array>
#include <vector>
#include "stdint.h"
#include <cassert>
#include "stdio.h"

namespace midi{

    class MidiPacket{
    public:
        MidiPacket(){payload = {0};}
        template<typename Iter>
        MidiPacket(Iter it){
            for(size_t i = 0; i < 4; i++){
                payload[i] = *it;
                it++;
            }
        }
        MidiPacket(std::initializer_list<uint8_t> values){
            printf("Midipacket initializer list");
            assert(values.size() < 5);
            for(auto val: values){
                printf("%d", val);
            }
            payload.reserve(values.size());
            std::copy(values.begin(), values.end(), payload.begin());
        }
        std::vector<uint8_t> payload;
    };


    namespace msg{

        enum MIDI_STATUS : uint8_t{
            NOTE_ON = 0x90,
            NOTE_OFF = 0x80,
            CC = 0xB0,
            PC = 0xC0,
            PITCH_BEND = 0xE0
        };

        MidiPacket noteOn(uint8_t channel, uint8_t note_value, uint8_t velocity);
    }

} // namespace midi