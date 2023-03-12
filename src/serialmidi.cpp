//
// Created by bjo on 1/7/23.
//

#include "../include/serialmidi.hpp"

BEGIN_MIDI_NAMESPACE
    void MySerialMIDI::begin() const {
        uart_init(uart0, SERIAL_BAUD);

        // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
        gpio_set_function(rxpin, GPIO_FUNC_UART);
        gpio_set_function(txpin, GPIO_FUNC_UART);
    }

    void MySerialMIDI::end() {
    }

    bool MySerialMIDI::beginTransmission(MidiType) {
        return true;
    };

    void MySerialMIDI::write(byte value) {
        uart_putc(uart0, value);
    };

    void MySerialMIDI::endTransmission() {
    };

    byte MySerialMIDI::read() {
        return uart_getc(uart0);
    };

    unsigned MySerialMIDI::available() {
        return uart_is_readable(uart0);
    };

END_MIDI_NAMESPACE
