//
// Created by bjo on 3/12/23.
//

#include <Arduino.h>
#include <cstdint>
#include <pico/time.h>
#include <hardware/gpio.h>
#include "ui.hpp"
#include "config.hpp"

bool CLKstate;
bool DTstate;

// Mega gross, but that works as long as you don't forget to initialize it before using it
UI *ui_for_encoder;
static uint8_t state=0;
// https://forum.arduino.cc/t/reading-rotary-encoders-as-a-state-machine/937388
void readEncoder() {
    CLKstate = digitalRead(ENC_B);
    DTstate = digitalRead(ENC_C);
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
                ui_for_encoder->encoder_right();
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
                ui_for_encoder->encoder_left();
            }
            break;
    }
}

unsigned long time_a = to_ms_since_boot(get_absolute_time());
int delayTime_a = 200;

void encoderUpdate() {
    CLKstate = digitalRead(ENC_B);
    DTstate = digitalRead(ENC_C);
}

void UI::initEncoder() {
    ui_for_encoder=this;
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    pinMode(ENC_C, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENC_B), encoderUpdate, RISING);
    attachInterrupt(digitalPinToInterrupt(ENC_C), encoderUpdate, RISING);

    interrupts();
}

#define CURRENT_TIME_MS to_ms_since_boot(get_absolute_time())


void UI::encoderPoll() {
    bool click = digitalRead(ENC_A);
    if (click == 0) {
        if ((CURRENT_TIME_MS - time_a) > delayTime_a) {
            time_a = CURRENT_TIME_MS;
            encoder_click();
        }
    }
    readEncoder();
}