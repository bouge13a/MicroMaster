/*
 * text_controls.c
 *
 *  Created on: Aug 3, 2020
 *      Author: steph
 */
#include <text_controls.hpp>
#include "uartstdio.h"

namespace TextCtl {

    void set_text_mode(uint32_t mode) {
        UARTprintf("\033[%dm", mode);
    }

    void text_color(uint32_t color) {
        UARTprintf("\033[0;%dm", color);
    }

    void bold_text_color(uint32_t color){
        UARTprintf("\033[1;%dm", color);
    }

    void bgd_color(uint32_t color) {
        UARTprintf("\033[%dm", color);
    }

    void cursor_pos(uint32_t row, uint32_t column) {
        UARTprintf("\033[%d;%dH", row, column);
    }

    void save_cursor_pos(void) {
        UARTprintf("\033[s");
    }

    void restore_cursor_pos(void) {
        UARTprintf("\033[u");
    }

    void clear_terminal(void){
        UARTprintf("\033[2J");
    }

    void clear_line(void){
        UARTprintf("\033[2K");
    }

    void clear_in_line(void){
        UARTprintf("\033[0K");
    }

    void clear_below_line(void){
        UARTprintf("\033[0J");
    }

}
