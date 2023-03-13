//
// Created by bjo on 3/11/23.
//
#include <cstdint>  // necessary for uint8_t
#include "routingmatrix.hpp"

#ifndef DINOCTOPUS_2040_MIDI_ROUTER_HPP
#define DINOCTOPUS_2040_MIDI_ROUTER_HPP

class MidiRouter {
public:
    void init();
    void loop();
    void get_matrix(routing_matrix &);
private:
    uint8_t current_cable_limited();
    routing_matrix matrix = {
            {0,0,0,255},
            {0,0,0,255},
            {0,0,0,255},
            {0,0,0,0}
    };
};

#endif //DINOCTOPUS_2040_MIDI_ROUTER_HPP
