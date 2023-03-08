#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include <SerialUART.h>
//#include "hardware/pio.h"
//#include "hardware/uart.h"
//#include "uart_rx.pio.h"
//#include "uart_tx.pio.h"
#include "MIDI.h"
#include "piomidi.hpp"
#include "serialmidi.hpp"
#include "usbcablemidi.hpp"

#define NEOPIXEL_PIN 9

#define LCD_CLOCK 7  // Clock (Common), sometimes called SCK or SCL
#define LCD_MOSI 6   // MOSI (common), sometimes called SDA or DATA
#define LCD_RESET 8   // LCD reset, sometimes called RST or RSTB
#define LCD_CS 2      // LCD CS, sometimes called EN or SS
#define LCD_RS 3      // LCD RS, sometimes called A0 or DC

// blue on 12
#define PIN_MIDI_1_RX 12
#define PIN_MIDI_1_TX 13

// blue on 14
#define PIN_MIDI_2_RX 14
#define PIN_MIDI_2_TX 15

// blue on 16
#define PIN_MIDI_3_RX 16
#define PIN_MIDI_3_TX 17

// blue on 18
#define PIN_MIDI_4_RX 18
#define PIN_MIDI_4_TX 19

// USB MIDI object
Adafruit_USBD_MIDI usb_midi(4); // number of virtual cables, you will need to define all the interfaces here
UsbJackMIDI jack_0(usb_midi, 0);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_1(jack_0);

UsbJackMIDI jack_1(usb_midi, 1);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_2(jack_1);

UsbJackMIDI jack_2(usb_midi, 2);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_3(jack_2);

UsbJackMIDI jack_3(usb_midi, 3);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_4(jack_3);

//PioMIDI pio_serial(pio_rx, pio_tx, offset_rx, offset_tx, 0, 0, PIN_MIDI_1_RX, PIN_MIDI_1_TX);
// TODO move the init of SerialPIO into PioMIDI!

PIO pio_rx = pio0;
PIO pio_tx = pio1;
uint offset_rx = pio_add_program(pio_rx, &uart_rx_program);
uint offset_tx = pio_add_program(pio_tx, &uart_tx_program);

//SerialPIO pio_serial(PIN_MIDI_1_TX, PIN_MIDI_1_RX , 32);
//PioMIDI pio_midi(pio_serial);
//MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_1(pio_midi);

//PioMIDI pio_serial2(pio_rx, pio_tx, offset_rx, offset_tx, 1, 1, PIN_MIDI_2_RX, PIN_MIDI_2_TX);
//SerialPIO pio_serial2(PIN_MIDI_2_TX, PIN_MIDI_2_RX , 32);
//PioMIDI pio_midi2(pio_serial2);
//MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_2(pio_midi2);

//PioMIDI pio_serial3(pio_rx, pio_tx, offset_rx, offset_tx, 2, 2, PIN_MIDI_3_RX, PIN_MIDI_3_TX);
//SerialPIO pio_serial3(PIN_MIDI_3_TX, PIN_MIDI_3_RX , 32);
//PioMIDI pio_midi3(pio_serial3);
//MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_3(pio_midi3);

//PioMIDI pio_serial4(pio_rx, pio_tx, offset_rx, offset_tx, 3, 3, PIN_MIDI_4_RX, PIN_MIDI_5_TX);
//SerialPIO pio_serial4(PIN_MIDI_4_TX, PIN_MIDI_4_RX , 32);
//PioMIDI pio_midi4(pio_serial4);
//MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_4(pio_midi4);

//MIDI_NAMESPACE::SerialMIDI<HardwareSerial> real_serial_0(Serial1);
MIDI_NAMESPACE::MySerialMIDI real_serial_0(PIN_MIDI_1_RX, PIN_MIDI_1_TX);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::MySerialMIDI> MIDI_IF_1(real_serial_0);

MIDI_NAMESPACE::PioMIDI pio_serial2(pio_rx, pio_tx, offset_rx, offset_tx, 0, 0, PIN_MIDI_2_RX, PIN_MIDI_2_TX);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::PioMIDI> MIDI_IF_2((MIDI_NAMESPACE::PioMIDI&)pio_serial2);

MIDI_NAMESPACE::PioMIDI pio_serial3(pio_rx, pio_tx, offset_rx, offset_tx, 1, 1, PIN_MIDI_3_RX, PIN_MIDI_3_TX);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::PioMIDI> MIDI_IF_3((MIDI_NAMESPACE::PioMIDI&)pio_serial3);

MIDI_NAMESPACE::PioMIDI pio_serial4(pio_rx, pio_tx, offset_rx, offset_tx, 2, 2, PIN_MIDI_4_RX, PIN_MIDI_4_TX);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::PioMIDI> MIDI_IF_4((MIDI_NAMESPACE::PioMIDI&)pio_serial4);

#define FILTER_NOTE_ON 1
#define FILTER_NOTE_OFF 2
#define FILTER_CC 4

// bits for filtering later
// 7    6    5    4    3    2      1             0
//                          CC        Note OFF     Note On
// we can also add channels with higher numbers and shifting, will see
int routing_matrix[4][4] = {
        {0,0,0,255},
        {0,0,0,255},
        {0,0,0,255},
        {0,0,0,0}
};

uint8_t current_cable_limited() {
    uint8_t cable = jack_0.current_cable();
    if (cable<1 | cable>3)
        cable = 0;
    return cable;
}

#define USB_DIRECT_TO_INTERFACE_FROM_CABLE(cmd) uint8_t cable = current_cable_limited(); \
    switch(cable) { \
        case 1: \
            MIDI_IF_2. cmd ; \
        case 2: \
            MIDI_IF_3. cmd ; \
        case 3: \
            MIDI_IF_4. cmd ; \
        default: \
            MIDI_IF_1. cmd ; \
        }

void usb_handleNoteOn(byte channel, byte note, byte velocity)
{
    USB_DIRECT_TO_INTERFACE_FROM_CABLE(sendNoteOn(note, velocity, channel));
}

void usb_handleNoteOff(byte channel, byte note, byte velocity)
{
    USB_DIRECT_TO_INTERFACE_FROM_CABLE(sendNoteOff(note, velocity, channel));
}

void usb_controlChange(byte channel, byte number, byte value) {
    USB_DIRECT_TO_INTERFACE_FROM_CABLE(sendControlChange(number, value, channel));
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_RGB+ NEO_KHZ800);
U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CLOCK,
                                          LCD_MOSI,
                                          LCD_CS,
                                          LCD_RS,
                                          LCD_RESET); // clock, data, cs, dc, reset


void initStrip() {
    strip.begin();
    strip.setBrightness(255);
    strip.setPixelColor(0, strip.Color(255, 25, 0));
    strip.setPixelColor(1, strip.Color(255, 25, 0));
    strip.setPixelColor(2, strip.Color(255, 25, 0));
    strip.show();
    delay(100);
    strip.setBrightness(255);
    strip.setPixelColor(0, strip.Color(1, 255, 85));
    strip.setPixelColor(1, strip.Color(1, 255, 145));
    strip.setPixelColor(2, strip.Color(15, 255, 45));
    strip.show();
}
bool usb_ready = false;
void setup() {
    TinyUSB_Device_Init(0);
    initStrip();

    // wait until device mounted

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


    u8g2_lcd.begin();
    u8g2_lcd.setContrast(180);  // This is extremely important
    u8g2_lcd.clearBuffer();
    u8g2_lcd.firstPage();
    u8g2_lcd.setFont(u8g2_font_6x12_tr);
    u8g2_lcd.setFontMode(0);
    u8g2_lcd.firstPage();
    do {
        //u8g2_lcd.drawHLine(0, 58, 128);
        u8g2_lcd.drawStr(0, 10, "A strange game.");
        u8g2_lcd.drawStr(0,20, "The only winning move");
        u8g2_lcd.drawStr(0,30," is not to play");
        //char c=42;
        //snprintf_P(buffer, sizeof(buffer), PSTR("%02x %04x"), c, cycle);
        //u8g2_lcd.drawStr(10, 10, buffer);
    } while (u8g2_lcd.nextPage());

}

template <typename T, typename U>
void copy_midi_data(midi::MidiInterface<T> &in, midi::MidiInterface<U> &out) {
    out.send(in.getType(), in.getData1(), in.getData2(), in.getChannel());
}

char buffer[50];

void writebuffer() {
    u8g2_lcd.firstPage();
    do {
        //u8g2_lcd.drawHLine(0, 58, 128);
        u8g2_lcd.drawStr(0, 10, buffer);
    } while (u8g2_lcd.nextPage());
}

void loop() {
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
