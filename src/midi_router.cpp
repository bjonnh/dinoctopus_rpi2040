//
// Created by bjo on 3/11/23.
//

#include <Adafruit_TinyUSB.h>
#include "midi_router.hpp"
#include "midi_Defs.h"
#include "config.hpp"

template <typename T, typename U>
void copy_midi_data(midi::MidiInterface<T> &in, midi::MidiInterface<U> &out) {
    out.send(in.getType(), in.getData1(), in.getData2(), in.getChannel());
}

#define FILTER_NOTE_ON 1
#define FILTER_NOTE_OFF 2
#define FILTER_CC 4

// bits for filtering later
// 7    6    5    4    3    2      1             0
//                          CC        Note OFF     Note On
// we can also add channels with higher numbers and shifting, will see

Adafruit_USBD_MIDI usb_midi(4); // number of virtual cables, you will need to define all the interfaces here
midi::UsbJackMIDI jack_0(usb_midi, 0);
midi::MidiInterface<midi::UsbJackMIDI> MIDI_USB_1(jack_0);

midi::UsbJackMIDI jack_1(usb_midi, 1);
midi::MidiInterface<midi::UsbJackMIDI> MIDI_USB_2(jack_1);

midi::UsbJackMIDI jack_2(usb_midi, 2);
midi::MidiInterface<midi::UsbJackMIDI> MIDI_USB_3(jack_2);

midi::UsbJackMIDI jack_3(usb_midi, 3);
midi::MidiInterface<midi::UsbJackMIDI> MIDI_USB_4(jack_3);

midi::MySerialMIDI real_serial_0(PIN_MIDI_1_RX, PIN_MIDI_1_TX);
midi::MidiInterface<midi::MySerialMIDI> MIDI_IF_1(real_serial_0);

PIO_SERIAL_MIDI(2, PIN_MIDI_2_TX, PIN_MIDI_2_RX);
PIO_SERIAL_MIDI(3, PIN_MIDI_3_TX, PIN_MIDI_3_RX);
PIO_SERIAL_MIDI(4, PIN_MIDI_4_TX, PIN_MIDI_4_RX);

uint8_t MidiRouter::current_cable_limited() {
    uint8_t cable = jack_0.current_cable();
    if (cable<1 | cable>3)
        cable = 0;
    return cable;
}

void MidiRouter::init() {
    TinyUSB_Device_Init(0);
    MIDI_USB_1.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_2.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_3.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_4.begin(MIDI_CHANNEL_OMNI);

    while (!TinyUSBDevice.mounted())
        delay(1);

    MIDI_IF_1.begin(MIDI_CHANNEL_OMNI);
    MIDI_IF_2.begin(MIDI_CHANNEL_OMNI);
    MIDI_IF_3.begin(MIDI_CHANNEL_OMNI);
    MIDI_IF_4.begin(MIDI_CHANNEL_OMNI);

}

void MidiRouter::loop() {
    if (MIDI_IF_1.read())
    {
        if (routing_matrix[0][0] > 0)
            copy_midi_data(MIDI_IF_1, MIDI_IF_1);
        if (routing_matrix[0][1] > 0)
            copy_midi_data(MIDI_IF_1, MIDI_IF_2);
        if (routing_matrix[0][2] > 0)
            copy_midi_data(MIDI_IF_1, MIDI_IF_3);
        if (routing_matrix[0][3] > 0)
            copy_midi_data(MIDI_IF_1, MIDI_IF_4);
        copy_midi_data(MIDI_IF_1, MIDI_USB_1);
    }

    if (MIDI_IF_2.read())
    {
        if (routing_matrix[1][0] > 0)
            copy_midi_data(MIDI_IF_2, MIDI_IF_1);
        if (routing_matrix[1][1] > 0)
            copy_midi_data(MIDI_IF_2, MIDI_IF_2);
        if (routing_matrix[1][2] > 0)
            copy_midi_data(MIDI_IF_2, MIDI_IF_3);
        if (routing_matrix[1][3] > 0)
            copy_midi_data(MIDI_IF_2, MIDI_IF_4);
        copy_midi_data(MIDI_IF_2, MIDI_USB_2);
    }

    if (MIDI_IF_3.read())
    {
        if (routing_matrix[2][0] > 0)
            copy_midi_data(MIDI_IF_3, MIDI_IF_1);
        if (routing_matrix[2][1] > 0)
            copy_midi_data(MIDI_IF_3, MIDI_IF_2);
        if (routing_matrix[2][2] > 0)
            copy_midi_data(MIDI_IF_3, MIDI_IF_3);
        if (routing_matrix[2][3] > 0)
            copy_midi_data(MIDI_IF_3, MIDI_IF_4);
        copy_midi_data(MIDI_IF_3, MIDI_USB_3);
    }

    if (MIDI_IF_4.read())
    {
        if (routing_matrix[3][0] > 0)
            copy_midi_data(MIDI_IF_4, MIDI_IF_1);
        if (routing_matrix[3][1] > 0)
            copy_midi_data(MIDI_IF_4, MIDI_IF_2);
        if (routing_matrix[3][2] > 0)
            copy_midi_data(MIDI_IF_4, MIDI_IF_3);
        if (routing_matrix[3][3] > 0)
            copy_midi_data(MIDI_IF_4, MIDI_IF_4);
        copy_midi_data(MIDI_IF_4, MIDI_USB_4);
    }

    // This one has callbacks and works for all of them
    // we just need to pick up the cable number
    if (MIDI_USB_1.read()) {
        uint8_t cable = current_cable_limited();
        midi::MidiType type = MIDI_USB_1.getType();
        unsigned char data1 = MIDI_USB_1.getData1();
        unsigned char data2 = MIDI_USB_1.getData2();
        unsigned char channel = MIDI_USB_1.getChannel();
        switch(cable) {
            case 1:
                MIDI_IF_2.send(type,data1,data2,channel);
                break;
            case 2:
                MIDI_IF_3.send(type,data1,data2,channel);
                break;
            case 3:
                MIDI_IF_4.send(type,data1,data2,channel);
                break;
            default:
                MIDI_IF_1.send(type,data1,data2,channel);
        }
    }
}
