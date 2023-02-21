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
class MySerialMIDI
{
public:
    MySerialMIDI(uint rxpin, uint txpin)
            : rxpin(rxpin), txpin(txpin)
    {
    };

public:
    static const bool thruActivated = false;

    void begin()
    {
        uart_init(uart0, SERIAL_BAUD);

        // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
        gpio_set_function(rxpin, GPIO_FUNC_UART);
        gpio_set_function(txpin, GPIO_FUNC_UART);
    }

    void end()
    {
    }

    bool beginTransmission(MidiType)
    {
        return true;
    };

    void write(byte value)
    {
        uart_putc(uart0, value);
    };

    void endTransmission()
    {
    };

    byte read()
    {
        return uart_getc(uart0);
    };

    unsigned available()
    {
        return uart_is_readable(uart0);
    };

private:
    uint rxpin;
    uint txpin;
};
END_MIDI_NAMESPACE


#endif //DINOCTOPUS_2040_PIOMIDI_HPP
