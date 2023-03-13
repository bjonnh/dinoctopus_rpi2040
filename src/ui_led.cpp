//
// Created by bjo on 3/12/23.
//



#include "ui.hpp"
#include "Adafruit_NeoPixel.h"
#include "config.hpp"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);


void UI::initStrip() {
    strip.begin();
    strip.setBrightness(255);
    strip.setPixelColor(0, Adafruit_NeoPixel::Color(255, 25, 0));
    strip.setPixelColor(1, Adafruit_NeoPixel::Color(255, 25, 0));
    strip.setPixelColor(2, Adafruit_NeoPixel::Color(255, 25, 0));
    strip.show();
    delay(1000);
    strip.setBrightness(255);
    strip.setPixelColor(0, Adafruit_NeoPixel::Color(1, 255, 85));
    strip.setPixelColor(1, Adafruit_NeoPixel::Color(1, 255, 145));
    strip.setPixelColor(2, Adafruit_NeoPixel::Color(15, 255, 45));
    strip.show();
}