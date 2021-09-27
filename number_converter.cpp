/*
 * number_converter.cpp
 *
 *  Created on: Sep 23, 2021
 *      Author: steph
 */

#include "number_converter.hpp"
#include "uartstdio.h"
#include "text_controls.hpp"
#include "utils.hpp"
#include <string.h>

static const uint32_t MENU_SPACING = 12;
static const uint32_t MENU_START_ROW = 6;

static const char* menu_constant[] = {
                                      "Decimal",
                                      "Hexadecimal",
                                      "Octal",
                                      "Binary",
};


NumConverter::NumConverter(void) : ConsolePage("Num Converter",
                                               portMAX_DELAY,
                                               false)  {

    this->menu_index = 0;
    this->byte_buffer_idx = 0;
    this->bin_string = new char[10];
    this->byte_buffer = new char[10];

} // End NumConverter::NumConverter

void NumConverter::draw_menu(uint32_t index) {


    TextCtl::text_color(TextCtl::white_text);
    TextCtl::set_text_mode(TextCtl::mode_reverse);

    for (uint32_t idx=0; idx<4; idx++) {

        if (index == idx) {
            TextCtl::text_color(TextCtl::blue_text);
            TextCtl::set_text_mode(TextCtl::mode_reverse);

        }

        TextCtl::cursor_pos(MENU_START_ROW, idx*MENU_SPACING);

        UARTprintf("%s", menu_constant[idx]);

        if (index == idx) {
            TextCtl::text_color(TextCtl::white_text);
            TextCtl::set_text_mode(TextCtl::mode_reverse);
        }
    }

    TextCtl::set_text_mode(TextCtl::mode_reverse_off);

    TextCtl::cursor_pos(MENU_START_ROW+5, this->byte_buffer_idx+1);

}

void NumConverter::draw_numbers(uint32_t number) {

    TextCtl::cursor_pos(MENU_START_ROW+1, 0*MENU_SPACING);
    TextCtl::clear_line();
    UARTprintf("%d", number);

    TextCtl::cursor_pos(MENU_START_ROW+1, 1*MENU_SPACING);
    UARTprintf("0x%x", number);

    TextCtl::cursor_pos(MENU_START_ROW+1, 2*MENU_SPACING);
    UARTprintf("0%d", decimalToOctal(number));

    TextCtl::cursor_pos(MENU_START_ROW+1, 3*MENU_SPACING);
    for (uint32_t index=0; index<sizeof(uint32_t); index++) {
        int_to_bin_string((number >> (8*index)) & 0x000000ff, (uint8_t*)this->bin_string);
        UARTprintf("0b%s ", this->bin_string);
    }

    TextCtl::cursor_pos(MENU_START_ROW+5, this->byte_buffer_idx+1);
}

void NumConverter::draw_page(void) {

    this->draw_numbers(0);
    TextCtl::cursor_pos(MENU_START_ROW, 0);
    draw_menu(this->menu_index);

    TextCtl::cursor_pos(MENU_START_ROW+4, 0);
    UARTprintf("Enter a number:");

    TextCtl::cursor_pos(MENU_START_ROW+5, 0);

}

void NumConverter::draw_data(void) {

}

void NumConverter::draw_input(int character) {

    if (character == 127) {
        if(this->byte_buffer_idx > 0) {
            this->byte_buffer_idx--;
            this->byte_buffer[this->byte_buffer_idx]=0;
            draw_numbers(atoi(this->byte_buffer));
            UARTprintf("\b");
            TextCtl::clear_in_line();
            return;
        }
    }

    switch (character) {
    case ArrowKeys::LEFT :

        if (this->menu_index == 0) {
            this->menu_index = 3;
        } else {
            this->menu_index--;
        }

        draw_menu(this->menu_index);
        memset(this->byte_buffer, 0, 10);
        draw_numbers(atoi(this->byte_buffer));
        TextCtl::cursor_pos(MENU_START_ROW+5, 0);
        this->byte_buffer_idx = 0;
        this->num_buffer = 0;

        break;
    case ArrowKeys::RIGHT :

        this->menu_index = (this->menu_index + 1) % 4;

        draw_menu(this->menu_index);
        memset(this->byte_buffer, 0, 10);
        draw_numbers(atoi(this->byte_buffer));
        TextCtl::cursor_pos(MENU_START_ROW+5, 0);
        this->byte_buffer_idx = 0;
        this->num_buffer = 0;

        break;
    default :
        break;
    }

    switch(this->menu_index) {
    case 0 :
        if (character >= '0' && character <= '9') {

            UARTprintf("%c", character);
            this->byte_buffer[this->byte_buffer_idx] = (char)character;
            this->byte_buffer_idx++;
            draw_numbers(atoi(this->byte_buffer));

        }

        break;
    case 1 :
        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

                this->num_buffer <<= 4;
                this->num_buffer |= (ascii_to_hex(character));
                UARTprintf("%c", character);
                draw_numbers(this->num_buffer);
                this->byte_buffer_idx++;

        }

        break;
    case 2 :
        if (character >= '0' && character <= '8') {

            UARTprintf("%c", character);
            this->num_buffer |= ascii_to_hex(character) << (4*this->byte_buffer_idx);
            this->byte_buffer_idx++;
            draw_numbers(octalToDecimal(this->num_buffer));

        }

        break;
    case 3:
        if (character == '0') {
            this->num_buffer = this->num_buffer << 1;
            UARTprintf("%c", character);
            this->byte_buffer_idx++;
            draw_numbers(this->num_buffer);
        }

        if (character == '1') {
            this->num_buffer |= (1 << this->byte_buffer_idx);
            UARTprintf("%c", character);
            this->byte_buffer_idx++;
            draw_numbers(this->num_buffer);
        }

        break;
    }

}

void NumConverter::draw_reset(void) {

    this->byte_buffer_idx = 0;
    this->num_buffer = 0;

    this->draw_numbers(0);
    this->menu_index = 0;
    this->draw_menu(0);
}

void NumConverter::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
