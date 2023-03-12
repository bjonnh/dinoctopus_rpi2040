//
// Created by bjo on 3/11/23.
//
#include <cstdint>  // necessary for uint8_t

#ifndef DINOCTOPUS_2040_MIDI_ROUTER_HPP
#define DINOCTOPUS_2040_MIDI_ROUTER_HPP

class MidiRouter {
public:
    void init();
    void loop();
private:
    uint8_t current_cable_limited();
    int routing_matrix[4][4] = {
            {0,0,0,255},
            {0,0,0,255},
            {0,0,0,255},
            {0,0,0,0}
    };
};

#endif //DINOCTOPUS_2040_MIDI_ROUTER_HPP
