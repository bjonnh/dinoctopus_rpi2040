//
// Created by bjo on 1/7/23.
//

#ifndef DINOCTOPUS_2040_PIOMIDI_HPP
#define DINOCTOPUS_2040_PIOMIDI_HPP
#include <Arduino.h>
#include <midi_Namespace.h>
#include "hardware/pio.h"
#include "uart_rx.pio.h"
#include "uart_tx.pio.h"
#include "midi_Defs.h"


#define SERIAL_BAUD 31250

BEGIN_MIDI_NAMESPACE
class PioMIDI
{
public:
    PioMIDI(PIO pio_rx, PIO pio_tx, uint offset_rx, uint offset_tx, uint sm_rx, uint sm_tx, uint rxpin, uint txpin)
            : pio_rx(pio_rx), pio_tx(pio_tx),
                offset_rx(offset_rx), offset_tx(offset_tx),
                sm_rx(sm_rx), sm_tx(sm_tx),
                rxpin(rxpin), txpin(txpin)
    {
    };

public:
    static const bool thruActivated = false;

    void begin()
    {
        uart_rx_program_init(pio_rx, sm_rx, offset_rx, rxpin, SERIAL_BAUD);
        uart_tx_program_init(pio_tx, sm_tx, offset_tx, txpin, SERIAL_BAUD);
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
        uart_tx_program_putc(pio_tx, sm_tx, value);
    };

    void endTransmission()
    {
    };

    byte read()
    {
        return uart_rx_program_getc(pio_rx, sm_rx);
    };

    unsigned available()
    {
        return !pio_sm_is_rx_fifo_empty(pio_rx, sm_rx);
    };

private:
    PIO pio_rx;
    PIO pio_tx;
    uint sm_rx;
    uint sm_tx;
    uint rxpin;
    uint txpin;
    uint offset_rx=0;
    uint offset_tx=0;
};
END_MIDI_NAMESPACE


#endif //DINOCTOPUS_2040_PIOMIDI_HPP
