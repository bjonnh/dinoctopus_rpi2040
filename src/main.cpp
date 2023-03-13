#include <Arduino.h>  // necessary for setup/loop
#include "midi_router.hpp"
#include "ui.hpp"
#include "queue.hpp"
#include "utils.hpp"
#include "config.hpp"

MidiRouter midi_router;
UI ui;



// Core 0, running the MIDI will only receive requests never send any
queue_t request_queue;
queue_t response_queue;

LATENCY_VARIABLES(0)
LATENCY_VARIABLES(1)


void setup() {
    LATENCY_INIT(0)
    queue_init(&request_queue, sizeof(queue_request_t), 2);
    queue_init(&response_queue, sizeof(queue_response_t), 2);
    midi_router.init();
}

void setup1() {
    LATENCY_INIT(1)
    ui.init();
}


void loop() {
    LATENCY_CALCULATOR(0);

    if (queue_get_level(&request_queue) > 0) {
        queue_request_t request;
        queue_remove_blocking(&request_queue, &request);
        if (request.code == REQUEST_LATENCY) {
            queue_response_t entry = {RESPONSE_LATENCY, processor_max_latency_0};
            queue_add_blocking(&response_queue, &entry);
        } else if (request.code == REQUEST_ROUTING) {
            queue_response_t entry = {RESPONSE_ROUTING};
            midi_router.get_matrix(entry.data);
            queue_add_blocking(&response_queue, &entry);
        }
    }

    midi_router.loop();
}

void loop1() {
    LATENCY_CALCULATOR(1);
    if (count_latency_1==N_LOOPS_MAX_LATENCY_CORE1)
        ui.set_latency(1, processor_max_latency_1);

    if (queue_get_level(&response_queue) > 0) {
        queue_response_t response;
        queue_remove_blocking(&response_queue, &response);
        if (response.code == RESPONSE_LATENCY) {
            ui.set_latency(0, response.data[0][0]);
        } else if (response.code == RESPONSE_ROUTING) {
            ui.set_routing_response(response.data);
        }
    }

    uint32_t query = ui.query_for_router();
    // Unless we are asking for something specific, let's just ask for latency
    if (query != 0) {
        queue_request_t entry = {REQUEST_ROUTING, query};
        queue_add_blocking(&request_queue, &entry);
    } else {
        queue_request_t entry = {REQUEST_LATENCY, 0};
        queue_add_blocking(&request_queue, &entry);
    }
    ui.loop();
}
