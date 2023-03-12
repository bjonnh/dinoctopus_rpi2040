//
// Created by bjo on 3/11/23.
//

#include <Arduino.h>
#include "ui.hpp"
#include "Adafruit_NeoPixel.h"
#include "U8g2lib.h"
#include "config.hpp"
#include <SPI.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);
/*U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CLOCK,
                                          LCD_MOSI,
                                          LCD_CS,
                                          LCD_RS,
                                          LCD_RESET); // clock, data, cs, dc, reset
*/
/*U8G2_ST7567_JLX12864_1_3W_SW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CLOCK,
                                          LCD_MOSI,
                                          LCD_CS,

                                          LCD_RESET);*/

U8G2_ST7567_JLX12864_F_4W_HW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CS,//LCD_CS,
                                          LCD_RS,
                                          LCD_RESET);

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

void UI::initLCD() {
    SPI.setRX(0);
    SPI.setCS(1);
    SPI.setSCK(LCD_CLOCK);
    SPI.setTX(LCD_MOSI);

    SPI.begin();
    SPI.beginTransaction(SPISettings(64000000, MSBFIRST, SPI_MODE0));
    u8g2_lcd.begin();
    u8g2_lcd.setContrast(180);  // This is extremely important
    u8g2_lcd.clearBuffer();
    u8g2_lcd.firstPage();
    u8g2_lcd.setFont(u8g2_font_6x12_tr);
    u8g2_lcd.setFontMode(0);
    u8g2_lcd.clearBuffer();
    u8g2_lcd.drawStr(0, 10, "A strange game.");
    u8g2_lcd.drawStr(0, 20, "The only winning move");
    u8g2_lcd.drawStr(0, 30, " is not to play");
    u8g2_lcd.sendBuffer();
}

int oldclickcount = 0;
int clickcount = 0;

bool CLKstate;
bool DTstate;

int16_t inputDelta = 0;             // Counts up or down depending which way the encoder is turned
static uint8_t state = 0;

// https://forum.arduino.cc/t/reading-rotary-encoders-as-a-state-machine/937388
void readEncoder() {
    switch (state) {
        // Start state
        case 0:
            if (!CLKstate) {             // Turn clockwise and CLK goes low first
                state = 1;
            } else if (!DTstate) {      // Turn anticlockwise and DT goes low first
                state = 4;
            }
            break;
        // Clockwise rotation
        case 1:
            if (!DTstate) {             // Continue clockwise and DT will go low after CLK
                state = 2;
            }
            break;
        case 2:
            if (CLKstate) {             // Turn further and CLK will go high first
                state = 3;
            }
            break;
        case 3:
            if (CLKstate && DTstate) {  // Both CLK and DT now high as the encoder completes one step clockwise
                state = 0;
                ++inputDelta;
            }
            break;
            // Anticlockwise rotation
        case 4:                         // As for clockwise but with CLK and DT reversed
            if (!CLKstate) {
                state = 5;
            }
            break;
        case 5:
            if (DTstate) {
                state = 6;
            }
            break;
        case 6:
            if (CLKstate && DTstate) {
                state = 0;
                --inputDelta;
            }
            break;
    }
}

unsigned long time_a = to_ms_since_boot(get_absolute_time());
int delayTime_a = 200;

void encoderMove(uint gpio, uint32_t events) {
    if ((to_ms_since_boot(get_absolute_time()) - time_a) > delayTime_a) {
        time_a = to_ms_since_boot(get_absolute_time());
        if ((gpio == ENC_A) && (events & GPIO_IRQ_EDGE_FALL))
            clickcount += 1;
    }

    if ((gpio == ENC_B) || (gpio == ENC_C)) {
        CLKstate = gpio_get(ENC_B);
        DTstate = gpio_get(ENC_C);
        readEncoder();
    }
}

void UI::initEncoder() {
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    pinMode(ENC_C, INPUT_PULLUP);
    gpio_set_irq_enabled_with_callback(ENC_A, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &encoderMove);
    CLKstate = digitalRead(ENC_B);
    DTstate = digitalRead(ENC_C);
    gpio_set_irq_enabled_with_callback(ENC_B, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &encoderMove);
    gpio_set_irq_enabled_with_callback(ENC_C, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &encoderMove);
    interrupts();
}

void UI::init() {
    initStrip();
    initLCD();
    initEncoder();
}

char buff[50];

void UI::writetext(char *s) {
    u8g2_lcd.clearBuffer();
    snprintf(buff, 50, "B: %d C: %d c: %d r: %d", CLKstate, DTstate, clickcount, inputDelta);
    u8g2_lcd.drawStr(0, 10, buff);

    u8g2_lcd.drawStr(0, 20, s);
    snprintf(buff, 50, "Q %4lx %4lx", query_code, query_value);
    u8g2_lcd.drawStr(0, 30, buff);
    snprintf(buff, 50, "L [0: %4d] [1: %4d]", latency_cpu[0], latency_cpu[1]);
    u8g2_lcd.drawStr(0, 60, buff);
    u8g2_lcd.sendBuffer();
}

int count = 0;
void UI::loop() {
    if (oldclickcount != clickcount) {
        query_router_for = clickcount;
        oldclickcount = clickcount;
    }

    snprintf(buffer, 50, "%x %x %x %x %x %x", count++, count, count, count ,count ,count);
    writetext(buffer);
}

void UI::set_latency(uint8_t cpu, uint32_t value) {
    latency_cpu[cpu] = value;
}

uint32_t UI::query_for_router() {
    uint32_t request = query_router_for;
    query_router_for = 0;
    return request;
}

void UI::set_query_response(uint32_t code, uint32_t value) {
    query_code = code;
    query_value = value;
}
