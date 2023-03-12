//
// Created by bjo on 1/7/23.
//

#ifndef DINOCTOPUS_2040_SERIALMIDI_HPP
#define DINOCTOPUS_2040_SERIALMIDI_HPP

#include <Arduino.h>
#include <midi_Namespace.h>
#include "hardware/uart.h"
#include <midi_Defs.h>


#define SERIAL_BAUD 31250

BEGIN_MIDI_NAMESPACE
    class MySerialMIDI {
    public:
        MySerialMIDI(uint rxpin, uint txpin)
                : rxpin(rxpin), txpin(txpin) {
        };

    public:
        static const bool thruActivated = false;

        void begin() const;

        void end();

        static bool beginTransmission(MidiType);

        static void write(byte value);

        void endTransmission();

        static byte read();

        static unsigned available();

    private:
        uint rxpin;
        uint txpin;
    };
END_MIDI_NAMESPACE


#endif //DINOCTOPUS_2040_PIOMIDI_HPP
