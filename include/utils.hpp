//
// Created by bjo on 3/12/23.
//

#ifndef DINOCTOPUS_2040_UTILS_HPP
#define DINOCTOPUS_2040_UTILS_HPP

#define LATENCY_VARIABLES(id) static uint32_t count_latency_##id = 0; \
    static uint32_t processor_time_##id; \
    static uint32_t processor_max_latency_##id = 0;

#define LATENCY_INIT(id) processor_time_##id =  to_us_since_boot(get_absolute_time()); \

#define LATENCY_CALCULATOR(id) uint32_t processor_current_time_##id = to_us_since_boot(get_absolute_time()); \
        uint32_t processor_latency_##id = processor_current_time_##id - processor_time_##id; \
        processor_time_##id = processor_current_time_##id; \
        if (processor_latency_##id > processor_max_latency_##id) \
            processor_max_latency_##id = processor_latency_##id; \
        if (count_latency_##id++ > N_LOOPS_MAX_LATENCY_CORE##id) { \
            count_latency_##id = 0; \
            processor_max_latency_##id = 0; \
        }


#endif //DINOCTOPUS_2040_UTILS_HPP
