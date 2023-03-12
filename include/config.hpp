//
// Created by bjo on 3/11/23.
//

#ifndef DINOCTOPUS_2040_CONFIG_HPP
#define DINOCTOPUS_2040_CONFIG_HPP

#include "MIDI.h"
#include "piomidi.hpp"
#include "serialmidi.hpp"
#include "usbcablemidi.hpp"

//
// LCD
//

#define NEOPIXEL_PIN 9

#define LCD_CS  5    // LCD CS, sometimes called EN or SS
#define LCD_RS 7   // LCD RS, sometimes called A0 or DC
#define LCD_CLOCK 2 // Clock (Common), sometimes called SCK or SCL real pin
#define LCD_MOSI 3 // MOSI (common), sometimes called SDA or DATA
#define LCD_RESET 8  // LCD reset, sometimes called RST or RSTBb


//
// MIDI
//

#define PIN_MIDI_1_RX 12
#define PIN_MIDI_1_TX 13

#define PIN_MIDI_2_RX 14
#define PIN_MIDI_2_TX 15

#define PIN_MIDI_3_RX 16
#define PIN_MIDI_3_TX 17

#define PIN_MIDI_4_RX 18
#define PIN_MIDI_4_TX 19

#define ENC_A 20
#define ENC_B 21
#define ENC_C 22

#endif //DINOCTOPUS_2040_CONFIG_HPP
