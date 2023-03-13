//
// Created by bjo on 3/11/23.
//

#include <Arduino.h>
#include "ui.hpp"
#include "U8g2lib.h"
#include "config.hpp"
#include <SPI.h>


U8G2_ST7567_JLX12864_F_4W_HW_SPI u8g2_lcd(U8G2_R2,
                                          LCD_CS,//LCD_CS,
                                          LCD_RS,
                                          LCD_RESET);


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
    u8g2_lcd.setFontMode(0);
    u8g2_lcd.clearBuffer();
    u8g2_lcd.drawStr(0, 10, "A strange game.");
    u8g2_lcd.drawStr(0, 20, "The only winning move");
    u8g2_lcd.drawStr(0, 30, " is not to play");
    u8g2_lcd.sendBuffer();
}

int oldclickcount = 0;
int clickcount = 0;

int16_t inputDelta = 0;             // Counts up or down depending which way the encoder is turned


void UI::init() {
    initStrip();
    initLCD();
    initEncoder();
}

char matrix_sign(uint32_t v) {
    return v > 0 ? 'X' : '.';
}

#define LINE_HEIGHT 8

uint8_t page = 0;
uint8_t cursor_position = 0;
uint8_t zone = 0; // 0 menu bar anything above is in the page itself and should be handled by it

// TODO make those pages as classes so they can receive events
// TODO make it able to go back to the previous zone if going to cursor 0
uint8_t p0_selected_input = 1;
uint8_t p0_selected_output = 0;

void UI::page0() {
    // zone 1: select input
    // zone 2: select output
    if (zone == 1) {
        p0_selected_input = cursor_position;
    } else if (zone == 2) {
        p0_selected_output = cursor_position;
    }
    u8g2_lcd.setFont(u8g2_font_5x8_mf);
    u8g2_lcd.drawStr(0, 2.5 * LINE_HEIGHT, "IO|   1   2   3   4 ");
    u8g2_lcd.drawHLine(0, 2.5 * LINE_HEIGHT, 100);
    for (uint8_t i = 0; i < 4; i++) {
        if (zone>0 && p0_selected_input == i+1)
            u8g2_lcd.setDrawColor(0);
        else
            u8g2_lcd.setDrawColor(1);
        snprintf(buffer, 3, "%d", i);
        u8g2_lcd.drawStr(0, (3.5 + i) * LINE_HEIGHT, buffer);
        u8g2_lcd.setDrawColor(1);
        u8g2_lcd.drawStr(10, (3.5 + i) * LINE_HEIGHT, "|");
        for (int j = 0; j<4 ; j++) {
            if ((p0_selected_output == j+1) && (p0_selected_input == i+1))
                u8g2_lcd.setDrawColor(0);
            else
                u8g2_lcd.setDrawColor(1);
            snprintf(buffer, 50, "   %c", matrix_sign(current_matrix[i][j]));
            u8g2_lcd.drawStr(15+20*j, (3.5 + i) * LINE_HEIGHT, buffer);
        }

    }
}

void UI::common() {
    u8g2_lcd.setFont(u8g2_font_5x8_mf);
    u8g2_lcd.setDrawColor(1);

    if (page == 0)
        if ((zone == 0) || cursor_position == 0)
            u8g2_lcd.setDrawColor(0);
        else {
            u8g2_lcd.drawHLine(0, LINE_HEIGHT + 1, 50);
        }
    u8g2_lcd.drawStr(0, LINE_HEIGHT, "ROUTING");
    u8g2_lcd.setDrawColor(1);

    if (page == 1)
        if (zone == 0 || cursor_position == 0)
            u8g2_lcd.setDrawColor(0);
        else {
            u8g2_lcd.drawHLine(50, LINE_HEIGHT + 1, 50);
        }
    u8g2_lcd.drawStr(50, LINE_HEIGHT, "SETTINGS");
    u8g2_lcd.setDrawColor(1);

    if (page == 2)
        if (zone == 0 || cursor_position == 0)
            u8g2_lcd.setDrawColor(0);
        else {
            u8g2_lcd.drawHLine(100, LINE_HEIGHT + 1, 50);
        }
    u8g2_lcd.drawStr(100, LINE_HEIGHT, "DEBUG");
    u8g2_lcd.setDrawColor(1);

    u8g2_lcd.setDrawColor(1);
    snprintf(buffer, 50, "L [0:%4d] [1:%4d]", latency_cpu[0], latency_cpu[1]);
    u8g2_lcd.drawStr(0, 8 * LINE_HEIGHT - 1, buffer);
}

void UI::display_update() {
    u8g2_lcd.clearBuffer();
    if (page == 0) {
        page0();
    }

    common();
    u8g2_lcd.sendBuffer();
}

int count = 0;

void UI::loop() {
    encoderPoll();
    if (oldclickcount != clickcount) {
        query_router_for = clickcount;
        oldclickcount = clickcount;
    }
    display_update();
}

void UI::set_latency(uint8_t cpu, uint32_t value) {
    latency_cpu[cpu] = value;
}

uint32_t UI::query_for_router() {
    uint32_t request = query_router_for;
    query_router_for = 0;
    return request;
}

void UI::set_routing_response(routing_matrix &new_matrix) {
    memcpy(current_matrix, new_matrix, sizeof(uint32_t) * 4 * 4);
}

void UI::encoder_right() {
    cursor_position += 1;
    if (zone == 0) {
        if (cursor_position > 2)
            cursor_position = 2;
        page = cursor_position;
    } else if (zone == 1) { // TODO This should go in page management
        if (page == 0)
            if (cursor_position > 4)
                cursor_position = 4;
    } else if (zone == 2) { // TODO This should go in the page management
        if (page == 0)
            if (cursor_position > 4)
                cursor_position = 4;
    }
}

void UI::encoder_left() {
    if (cursor_position > 0)
        cursor_position -= 1;

    if (zone == 0) {
        page = cursor_position;
    }
}

void UI::encoder_click() {
    if (zone>0 && cursor_position==0) {
        zone = 0;
        cursor_position = page;
        return;
    }

    if (zone == 0) {
        zone = 1;
        cursor_position = 1;
    } else if (zone == 1) {
      zone = 2;
      cursor_position = 1;
    } else if (zone == 2) { // TODO This should go in page  management
        current_matrix[p0_selected_input-1][p0_selected_output-1] ^= 255;
    }


}
