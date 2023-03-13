//
// Created by bjo on 3/12/23.
//

#ifndef DINOCTOPUS_2040_QUEUE_HPP
#define DINOCTOPUS_2040_QUEUE_HPP

#include <pico/util/queue.h>
#include "routingmatrix.hpp"

enum queue_request{REQUEST_LATENCY, REQUEST_ROUTING, REQUEST_SET_ROUTING };
enum queue_response{RESPONSE_LATENCY, RESPONSE_ROUTING, RESPONSE_SET_ROUTING };


typedef struct
{
    queue_request code;
    routing_matrix data;
} queue_request_t;

typedef struct
{
    queue_response code;
    routing_matrix data;
} queue_response_t;

#endif //DINOCTOPUS_2040_QUEUE_HPP
