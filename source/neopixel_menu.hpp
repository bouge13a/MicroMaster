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


typedef enum {
    NEOPIX_RIGHT_DIR,
    NEOPIX_LEFT_DIR,
    NEOPIX_NO_DIR,
}neopix_dir_e;

typedef enum {
    OFF_MODE,
    SOLID_BLINK_MODE,
    CHAOS_MODE,
    STREAM_MODE,
    MOOD_MODE,
    RAINBOW_MODE,
    STROBE_MODE,
}neopix_modes_e;

typedef enum {
    NEOPIX_GREEN,
    NEOPIX_RED,
    NEOPIX_BLUE,
    NEOPIX_ALL,
}neopix_colors_e;

typedef enum {
    RAINBOW_START,
    RAINBOW_TO_GREEN,
    RAINBOW_GREEN,
    RAINBOW_YELLOW,
    RAINBOW_RED,
    RAINBOW_PURPLE,
    RAINBOW_BLUE,
    RAINBOW_CYAN,
    RAINBOW_FADE,
}rainbow_states_e;


class NeopixRow {
public:
    NeopixRow(void (*callback)(uint32_t index, neopix_dir_e direction, bool selected),
              uint32_t num_of_items,
              const char* name);

    void (*callback)(uint32_t index, neopix_dir_e direction, bool selected);
    uint32_t num_of_items;
    uint32_t menu_index;
    const char* name;
};

#ifdef __cplusplus
extern "C" {
#endif

    class NeopixelMenu : public ConsolePage {
    public:
        NeopixelMenu(NeopixelCtl* neopix_cmd);
    private :

        void task(NeopixelMenu* this_ptr);
        static void taskfunwrapper(void* parm);

        void change_brightness(uint32_t* value,
                               uint32_t brightness);

        void incr_color_brightness(uint32_t* value,
                                   neopix_colors_e color,
                                   int32_t incr);

        NeopixelCtl* neopix_cmd;

        std::vector<NeopixRow*> menu_rows;
        std::vector<uint32_t> colors;

        NeopixMsg* neopix_msg;
        NeopixMsg* neopix_clr_msg;
        NeopixMsg* neopix_stream_msg;
        NeopixMsg* neopix_rainbow_msg;

        uint32_t vert_menu_index;

        rainbow_states_e rainbow_state;
        rainbow_states_e rainbow_stream_state;
        uint32_t rainbow_counter;

        uint32_t* neopix_values;
        uint8_t* brightness_values;
        uint32_t brightness_counter;
        neopix_dir_e brightness_counter_dir;

        uint32_t solid_mode_counter;

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
