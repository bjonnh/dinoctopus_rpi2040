//
// Created by bjo on 3/11/23.
//

#ifndef DINOCTOPUS_2040_UI_HPP
#define DINOCTOPUS_2040_UI_HPP


#include "routingmatrix.hpp"

class UI {
public:
    void init();
    void loop();

    void set_latency(uint8_t cpu, uint32_t value);

    uint32_t query_for_router();

    void set_routing_response(routing_matrix &matrix);

    void encoder_click();
    void encoder_right();
    void encoder_left();
private:
    char buffer[50];

    void initStrip();

    void initLCD();

    void initEncoder();
    void display_update();
    uint8_t latency_cpu[2]={0,0};

    int query_router_for = 1; // We request an update when starting

    routing_matrix current_matrix;

    void page0();

    void common();

    void encoderPoll();
};


#endif //DINOCTOPUS_2040_UI_HPP
