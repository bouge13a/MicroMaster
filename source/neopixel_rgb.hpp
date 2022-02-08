/*
 * neopixel_rgb.hpp
 *
 *  Created on: Oct 20, 2021
 *      Author: steph
 */

#ifndef NEOPIXEL_RGB_HPP_
#define NEOPIXEL_RGB_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"
#include "neopixel_command.hpp"


typedef union {
    uint32_t data;
    uint8_t bytes[4];
}neopix_data_u;

#ifdef __cplusplus
extern "C" {
#endif

    class NeopixelRgb : public ConsolePage {
    public:
        NeopixelRgb(NeopixelCtl* neopix_cmd);
    private :

        void draw_values(uint32_t index);

        NeopixelCtl* neopix_cmd;
        NeopixMsg* neopix_msg;
        neopix_data_u neopix_data;

        uint32_t rgb_index;

        uint32_t green_val;
        uint32_t red_val;
        uint32_t blue_val;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif



#endif /* NEOPIXEL_RGB_HPP_ */
