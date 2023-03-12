//
// Created by bjo on 1/7/23.
//

#ifndef DINOCTOPUS_2040_USBCABLEMIDI_HPP
#define DINOCTOPUS_2040_USBCABLEMIDI_HPP

#include <midi/Adafruit_USBD_MIDI.h>
#include <utility>
#include "midi_Defs.h"

typedef struct {
    uint8_t buffer[4];
    uint8_t index;
    uint8_t total;
} midid_stream_t;
BEGIN_MIDI_NAMESPACE
    class UsbJackMIDI {
    public:
        explicit UsbJackMIDI(Adafruit_USBD_MIDI usb_midi, int cable)
                : usb_midi(std::move(usb_midi)), cable(cable) {
        };

    public:
        static const bool thruActivated = false;

        void begin();

        void end();

        static bool beginTransmission(midi::MidiType);

        void write(uint8_t value);

        void endTransmission();

        uint32_t tud_midi_n_stream_read_cable_aware(void *buffer, uint32_t bufsize);

        byte read();

        uint32_t tud_midi_n_available();

        unsigned available();

        [[nodiscard]] uint8_t current_cable() const;

    private:
        Adafruit_USBD_MIDI usb_midi;
        uint8_t cable = 0;
        midid_stream_t stream_read = {{0, 0, 0, 0}, 0, 0};
    };
END_MIDI_NAMESPACE
#endif //DINOCTOPUS_2040_USBCABLEMIDI_HPP
