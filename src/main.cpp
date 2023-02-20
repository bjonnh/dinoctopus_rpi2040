#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
//#include "hardware/pio.h"
//#include "hardware/uart.h"
//#include "uart_rx.pio.h"
//#include "uart_tx.pio.h"
#include "MIDI.h"
#include "piomidi.hpp"
#include "usbcablemidi.hpp"

#define NEOPIXEL_PIN 9

#define LCD_CLOCK 7  // Clock (Common), sometimes called SCK or SCL
#define LCD_MOSI 6   // MOSI (common), sometimes called SDA or DATA
#define LCD_RESET 8   // LCD reset, sometimes called RST or RSTB
#define LCD_CS 2      // LCD CS, sometimes called EN or SS
#define LCD_RS 3      // LCD RS, sometimes called A0 or DC


#define PIN_MIDI_1_RX 12
#define PIN_MIDI_1_TX 13

#define PIN_MIDI_2_RX 14
#define PIN_MIDI_2_TX 15

#define PIN_MIDI_3_RX 16
#define PIN_MIDI_3_TX 17

#define PIN_MIDI_4_RX 18
#define PIN_MIDI_4_TX 19

// USB MIDI object
Adafruit_USBD_MIDI usb_midi(4); // number of virtual cables, you will need to define all the interfaces here
UsbJackMIDI jack_0(usb_midi);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_1(jack_0);

UsbJackMIDI jack_1(usb_midi);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_2(jack_1);

UsbJackMIDI jack_2(usb_midi);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_3(jack_2);

UsbJackMIDI jack_3(usb_midi);
MIDI_NAMESPACE::MidiInterface<UsbJackMIDI> MIDI_USB_4(jack_3);

//PioMIDI pio_serial(pio_rx, pio_tx, offset_rx, offset_tx, 0, 0, PIN_MIDI_1_RX, PIN_MIDI_1_TX);
// TODO move the init of SerialPIO into PioMIDI!
SerialPIO pio_serial(PIN_MIDI_1_TX, PIN_MIDI_1_RX , 32);
PioMIDI pio_midi(pio_serial);
MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_1(pio_midi);

//PioMIDI pio_serial2(pio_rx, pio_tx, offset_rx, offset_tx, 1, 1, PIN_MIDI_2_RX, PIN_MIDI_2_TX);
SerialPIO pio_serial2(PIN_MIDI_2_TX, PIN_MIDI_2_RX , 32);
PioMIDI pio_midi2(pio_serial2);
MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_2(pio_midi2);

//PioMIDI pio_serial3(pio_rx, pio_tx, offset_rx, offset_tx, 2, 2, PIN_MIDI_3_RX, PIN_MIDI_3_TX);
SerialPIO pio_serial3(PIN_MIDI_3_TX, PIN_MIDI_3_RX , 32);
PioMIDI pio_midi3(pio_serial3);
MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_3(pio_midi3);

//PioMIDI pio_serial4(pio_rx, pio_tx, offset_rx, offset_tx, 3, 3, PIN_MIDI_4_RX, PIN_MIDI_5_TX);
SerialPIO pio_serial4(PIN_MIDI_4_TX, PIN_MIDI_4_RX , 32);
PioMIDI pio_midi4(pio_serial4);
MIDI_NAMESPACE::MidiInterface<PioMIDI> MIDI_IF_4(pio_midi4);

/**
 * Ok so this is a bit tricky. When using USB any read, will read on every cable. So what we do is store the
 * cable number when we get a packet. It is effectively a global variable so treat it with the utmost respect.
 * @return
 */
midi::MidiInterface<PioMIDI> *get_interface()
{
    uint8_t cable = jack_0.current_cable();
    midi::MidiInterface<PioMIDI> *midiInterface;
    switch(cable) {
        case 1:
            midiInterface = &MIDI_IF_2;
            break;
        case 2:
            midiInterface = &MIDI_IF_3;
            break;
        case 3:
            midiInterface = &MIDI_IF_4;
            break;
        default:
            midiInterface = &MIDI_IF_1;
            break;
    }
    return midiInterface;
}

void usb_handleNoteOn(byte channel, byte note, byte velocity)
{
    get_interface()->sendNoteOn(note, velocity, channel);
}

void usb_handleNoteOff(byte channel, byte note, byte velocity)
{
    get_interface()->sendNoteOff(note, velocity, channel);
}

void usb_controlChange(byte channel, byte number, byte value) {
    get_interface()->sendControlChange(number, value, channel);
}


int r0=0,r1=0,r2=0;
int g0=0,g1=0,g2=0;
int b0=0,b1=0,b2=0;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_GRB+ NEO_KHZ800);
U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CLOCK,
                                          LCD_MOSI,
                                          LCD_CS,
                                          LCD_RS,
                                          LCD_RESET); // clock, data, cs, dc, reset


void setColors() {
    r0+=5;
    if (r0>55) { r1+=5; r0=25; }
    if (r1>55) { g0+=5 ; r1=25; }
    if (g0>55) { g1+=5; g0=25; }
    if (g1>55) { b0+=5; g1=25; }
    if (b0>55) { b1+=5; b0=25; }
    if (b1>55) { b1=25; }
    strip.clear();
    strip.setPixelColor(0, strip.Color(g0, r0, b0));
    strip.setPixelColor(1, strip.Color(g1, r1, b1));
    strip.setPixelColor(2, strip.Color(155, 255, 150));
    strip.show();
}

void initStrip() {
    strip.begin();
    strip.setBrightness(255);
    strip.setPixelColor(0, strip.Color(15, 25, 0*25));
    strip.setPixelColor(1, strip.Color(15, 25, 0*25));
    strip.setPixelColor(2, strip.Color(155, 255, 0*255));
    strip.show();
}



void setup() {
    TinyUSB_Device_Init(0);
    initStrip();

    MIDI_USB_1.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_2.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_3.begin(MIDI_CHANNEL_OMNI);
    MIDI_USB_4.begin(MIDI_CHANNEL_OMNI);

    // wait until device mounted
    while (!TinyUSBDevice.mounted())
        delay(1);
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
        u8g2_lcd.drawStr(0, 0, "Started");
    } while (u8g2_lcd.nextPage());

    MIDI_USB_1.setHandleNoteOn(usb_handleNoteOn);
    MIDI_USB_1.setHandleNoteOff(usb_handleNoteOff);
    MIDI_USB_1.setHandleControlChange(usb_controlChange);
    MIDI_IF_1.begin();
    MIDI_IF_2.begin();
    MIDI_IF_3.begin();
    MIDI_IF_4.begin();
}

template <typename T, typename U>
void copy_midi_data(midi::MidiInterface<T> &in, midi::MidiInterface<U> &out) {
    out.send(in.getType(), in.getData1(), in.getData2(), in.getChannel());
}

void loop() {
    // All the four physical interfaces are sent to the virtual USB output interfaces (so you can access those inputs individually)
    // All the four physical interfaces are merged into the first physical midi interface output (real midi merging)
    // All the four virtual USB input interfaces will output to their respective physical interfaces

    // MIDI_IN_1 -+--+-+-+--> MIDI_OUT_1
    //            |  | | |
    // USB_IN_1  -+  | | |
    //               | | |
    // MIDI_IN_2 -+--+-|-|--> MIDI_OUT_2
    //            |    | |
    // USB_IN_2  -+    | |
    //                 | |
    // MIDI_IN_3 -+----+-|--> MIDI_OUT_3
    //            |      |
    // USB_IN_3  -+      |
    //                   |
    // MIDI_IN_4 -+------+--> MIDI_OUT_4
    //            |
    // USB_IN_4  -+

    if (MIDI_IF_1.read())
    {
        copy_midi_data(MIDI_IF_1, MIDI_IF_1);
        copy_midi_data(MIDI_IF_1, MIDI_USB_1);
    }

    if (MIDI_IF_2.read())
    {
        copy_midi_data(MIDI_IF_2, MIDI_IF_1);
        copy_midi_data(MIDI_IF_2, MIDI_USB_2);
    }

    if (MIDI_IF_3.read())
    {
        copy_midi_data(MIDI_IF_3, MIDI_IF_1);
        copy_midi_data(MIDI_IF_3, MIDI_USB_3);
    }

    if (MIDI_IF_4.read())
    {
        copy_midi_data(MIDI_IF_4, MIDI_IF_1);
        copy_midi_data(MIDI_IF_4, MIDI_USB_4);
    }

    MIDI_USB_1.read(); // This one has callbacks
}
