//
// Created by bjo on 1/7/23.
//

#ifndef DINOCTOPUS_2040_PIOMIDI_HPP
#define DINOCTOPUS_2040_PIOMIDI_HPP
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "midi_Defs.h"

#define SERIAL_BAUD 31250

class PioMIDI
{
public:
    explicit PioMIDI(const SerialPIO& serialPio)
            : serialPio(serialPio)
    {
    };

public:
    static const bool thruActivated = false;

    void begin()
    {
        serialPio.begin(SERIAL_BAUD);
    }

    void end()
    {
    }

    static bool beginTransmission(midi::MidiType)
    {
        return true;
    };

    void write(byte value)
    {
        serialPio.write(value);
    };

    void endTransmission()
    {
    };

    byte read()
    {
        return serialPio.read();
    };

    unsigned available()
    {
        return serialPio.available();
    };

private:
    SerialPIO serialPio;
};

#endif //DINOCTOPUS_2040_PIOMIDI_HPP
