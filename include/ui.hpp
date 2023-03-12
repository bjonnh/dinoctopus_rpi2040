//
// Created by bjo on 3/11/23.
//

#ifndef DINOCTOPUS_2040_UI_HPP
#define DINOCTOPUS_2040_UI_HPP


class UI {
public:
    void init();
    void loop();

    void set_self_latency(uint8_t i);

    void set_latency(uint8_t cpu, uint32_t value);

    uint32_t query_for_router();

    void set_query_response(uint32_t code, uint32_t value);

private:
    char buffer[50];

    void initStrip();

    void initLCD();

    void initEncoder();
    void writetext(char *);
    uint8_t latency_cpu[2]={0,0};

    int query_router_for = 0;
    uint32_t query_code = 0;
    uint32_t query_value= 0;
};


#endif //DINOCTOPUS_2040_UI_HPP
