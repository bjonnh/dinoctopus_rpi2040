//
// Created by bjo on 1/7/23.
//

#ifndef DINOCTOPUS_2040_PIOMIDI_HPP
#define DINOCTOPUS_2040_PIOMIDI_HPP

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "midi_Defs.h"

#define PIO_SERIAL_MIDI(number, tx_pin, rx_pin) SerialPIO pio_serial##number(tx_pin, rx_pin, 32); \
    MIDI_NAMESPACE::PioMIDI pio_midi##number(pio_serial##number); \
    MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::PioMIDI> MIDI_IF_##number(pio_midi##number);

#define SERIAL_BAUD 31250
#pragma once
BEGIN_MIDI_NAMESPACE
    class PioMIDI {
    public:
        explicit PioMIDI(const SerialPIO &serialPio)
                : serialPio(serialPio) {
        };

        static const bool thruActivated = false;

        void begin();

        void end();

        static bool beginTransmission(midi::MidiType);

        void write(byte value);

        void endTransmission();

        byte read();

        unsigned available();

    private:
        SerialPIO serialPio;
    };
END_MIDI_NAMESPACE
#endif //DINOCTOPUS_2040_PIOMIDI_HPP
