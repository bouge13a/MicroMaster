/*
 * number_converter.cpp
 *
 *  Created on: Sep 23, 2021
 *      Author: steph
 */

#include "number_converter.hpp"
#include "uartstdio.h"
#include "text_controls.hpp"

static const uint32_t MENU_SPACING = 20;

static const char* menu_constant[] = {
                                      "Decimal",
                                      "Hexadecimal",
                                      "Binary",
                                      "Octal",
};

//
//NumConverter::NumConverter(void) {
//
//    this->menu_index = 0;
//
//} // End NumConverter::NumConverter
//
//void NumConverter::draw_menu(uint32_t index) {
//
//    for (uint32_t idx=0; idx<4; idx++) {
//
//        if (index == idx) {
//            TextCtl::text_color(TextCtl::blue_text)
//            TextCtl::set_text_mode(TextCtl::mode_reverse);
//        }
//
//        TextCtl::cursor_pos(5, idx*MENU_SPACING);
//
//        UARTprintf("%s", menu_constant[idx])
//
//        if (index == idx) {
//            TextCtl::text_color(TextCtl::white_text);
//            TextCtl::set_text_mode(TextCtl::mode_reverse_off);
//        }
//    }
//
//}
//
//void NumConverter::draw_page(void) {
//
//}
//
//void NumConverter::draw_data(void) {
//
//}
//
//void NumConverter::draw_input(int character) {
//
//}
//
//void NumConverter::draw_reset(void) {
//
//}
//
//void NumConverter::draw_help(void) {
//
//    UARTprintf("THIS IS A HELP PAGE\r\n");
//
//}
