#include <Arduino.h>  // necessary for setup/loop
#include "midi_router.hpp"
#include "ui.hpp"

MidiRouter midi_router;
UI ui;

uint8_t processor_0_time = 0;
uint8_t processor_0_latency = 0;

uint8_t processor_1_time = 0;
uint8_t processor_1_latency = 0;


void setup() {
    processor_0_time = to_us_since_boot(get_absolute_time());
    midi_router.init();
}

void setup1() {
    processor_1_time = to_us_since_boot(get_absolute_time());
    ui.init();
}

int count_latency_0 = 0;

void loop() {
    processor_0_latency = to_us_since_boot(get_absolute_time()) - processor_0_time;
    uint32_t current_latency = to_us_since_boot(get_absolute_time());
    if (current_latency > processor_0_time)
        processor_0_time = current_latency;
    count_latency_0++;
    if (count_latency_0 > 1000) {
        count_latency_0=0;
        bool fifo_success = rp2040.fifo.push_nb(0x0); // 0x0 is latency of other CPU
        if (fifo_success)
            rp2040.fifo.push_nb(processor_0_latency);
    }
    if (rp2040.fifo.available() >= 2) {
        uint32_t code = rp2040.fifo.pop();
        uint32_t value = rp2040.fifo.pop();
        if (code == 0x1000) {
            bool fifo_success = rp2040.fifo.push_nb(code);
            if (fifo_success)
                rp2040.fifo.push_nb(value);
        }
        if ((rp2040.fifo.available()%2)==1)
            rp2040.fifo.pop(); // We try to catch back
    }

    midi_router.loop();
}

int count_latency_1 = 0;
void loop1() {
    processor_1_latency = to_us_since_boot(get_absolute_time()) - processor_1_time;
    uint32_t current_latency = to_us_since_boot(get_absolute_time());
    if (current_latency > processor_0_time)
        processor_1_time = current_latency;
    count_latency_1++;
    if (count_latency_1 > 100) {
        count_latency_1=0;
        ui.set_latency(1, processor_1_time);
    }
    if (rp2040.fifo.available() >= 2) {
        uint32_t code = rp2040.fifo.pop();
        uint32_t value = rp2040.fifo.pop();
        if (code == 0x0)
            ui.set_latency(0, value);
        else if (code == 0x1000) {
            ui.set_query_response(code, value);
        }
        if ((rp2040.fifo.available()%2)==1)
           rp2040.fifo.pop(); // We try to catch back
    }
    uint32_t query = ui.query_for_router();
    if (query!=0) {
        bool fifo_success = rp2040.fifo.push_nb(0x1000); // 0x100x is tell me about routing of port x
        if (fifo_success)
            rp2040.fifo.push_nb(query);
    }
    ui.loop();
}
