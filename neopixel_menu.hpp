/*
 * neopixel_menu.hpp
 *
 *  Created on: Oct 21, 2021
 *      Author: steph
 */

#ifndef NEOPIXEL_MENU_HPP_
#define NEOPIXEL_MENU_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"
#include "neopixel_command.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class NeopixelMenu : public ConsolePage {
    public:
        NeopixelMenu(void);
    private :

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* NEOPIXEL_MENU_HPP_ */
