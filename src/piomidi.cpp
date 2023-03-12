//
// Created by bjo on 1/7/23.
//

#include "../include/piomidi.hpp"

BEGIN_MIDI_NAMESPACE
    void PioMIDI::begin() {
        serialPio.begin(SERIAL_BAUD);
    }

    void PioMIDI::end() {
    }

    bool PioMIDI::beginTransmission(midi::MidiType) {
        return true;
    };

    void PioMIDI::write(byte value) {
        serialPio.write(value);
    };

    void PioMIDI::endTransmission() {
    };

    byte PioMIDI::read() {
        return serialPio.read();
    };

    unsigned PioMIDI::available() {
        return serialPio.available();
    };
END_MIDI_NAMESPACE
