/*
 * neopixel_rgb.cpp
 *
 *  Created on: Oct 20, 2021
 *      Author: steph
 */


#include "neopixel_rgb.hpp"

#include "uartstdio.h"
#include "text_controls.hpp"

static const uint32_t COLOR_START_ROW = 10;
static const uint32_t COLOR_START_COL = 25;
static const uint32_t COLOR_SPACING = 10;

static const uint32_t NUM_OF_COLORS = 3;


NeopixelRgb::NeopixelRgb(NeopixelCtl* neopix_cmd) : ConsolePage("RGB Control",
                                                    portMAX_DELAY,
                                                    false) {

    this->neopix_cmd = neopix_cmd;

    this->neopix_data.data = 0;
    this->rgb_index = 0;

    this->green_val = 0;
    this->red_val = 0;
    this->blue_val = 0;

    this->neopix_msg = new NeopixMsg(neopix_normal_msg);
    this->neopix_msg->num_tx_msgs = 1;
    this->neopix_msg->tx_msgs = &this->neopix_data.data;
    this->neopix_msg->msg_state = neopix_idle;

} // End NeopixelRgb

void NeopixelRgb::draw_values(uint32_t index) {

    assert(index<3);

    TextCtl::cursor_pos(COLOR_START_ROW+1, 0);
    TextCtl::clear_line();

    for(uint32_t idx=0; idx<NUM_OF_COLORS; idx++) {
        TextCtl::cursor_pos(COLOR_START_ROW+1, COLOR_START_COL + idx*COLOR_SPACING);

        if(index == idx) {
            TextCtl::set_text_mode(TextCtl::mode_reverse);
        }

        switch(idx) {
        case 0:
            UARTprintf("%d", this->green_val);
            break;
        case 1:
            UARTprintf("%d", this->red_val);
            break;
        case 2:
            UARTprintf("%d", this->blue_val);
            break;
        default :
            assert(0);
            break;
        }


        if(index == idx) {
            TextCtl::set_text_mode(TextCtl::mode_reverse_off);
        }

    }

} // End NeopixelRgb::draw_values

void NeopixelRgb::draw_page(void) {

    UARTprintf("Use left and right arrows to select color and up and down to toggle");


    TextCtl::cursor_pos(COLOR_START_ROW, COLOR_START_COL + 0*COLOR_SPACING);
    TextCtl::bold_text_color(TextCtl::green_text);
    UARTprintf("Green");

    TextCtl::cursor_pos(COLOR_START_ROW, COLOR_START_COL + 1*COLOR_SPACING);
    TextCtl::bold_text_color(TextCtl::red_text);
    UARTprintf("Red");

    TextCtl::cursor_pos(COLOR_START_ROW, COLOR_START_COL + 2*COLOR_SPACING);
    TextCtl::bold_text_color(TextCtl::blue_text);
    UARTprintf("Blue");

    TextCtl::bold_text_color(TextCtl::white_text);


    this->neopix_data.data = 0x00000000;
    this->neopix_cmd->add_msg(this->neopix_msg);

    this->draw_values(0);


}
void NeopixelRgb::draw_data(void) {

}
void NeopixelRgb::draw_input(int character) {

    switch (character) {
    case ArrowKeys::DOWN :

            if (this->rgb_index == 0) {
                if(this->green_val == 0) break;
                this->green_val--;
            } else if (this->rgb_index == 1) {
                if(this->red_val == 0) break;
                this->red_val--;
            } else {
                if(this->blue_val == 0) break;
                this->blue_val--;
            }


        this->draw_values(this->rgb_index);

        *this->neopix_msg->tx_msgs = (this->blue_val << 16) | (this->red_val << 8) | this->green_val;

        this->neopix_cmd->add_msg(this->neopix_msg);

        break;
    case ArrowKeys::UP :

            if (this->rgb_index == 0) {
                if(this->green_val == 255) break;
                this->green_val++;
            } else if (this->rgb_index == 1) {
                if(this->red_val == 255) break;
                this->red_val++;
            } else {
                if(this->blue_val == 255) break;
                this->blue_val++;
            }


        this->draw_values(this->rgb_index);

        *this->neopix_msg->tx_msgs = (this->blue_val << 16) | (this->red_val << 8) | this->green_val;

        this->neopix_cmd->add_msg(this->neopix_msg);

        break;
    case ArrowKeys::RIGHT :

        this->rgb_index = (this->rgb_index + 1) % NUM_OF_COLORS;
        this->draw_values(this->rgb_index);

        break;
    case ArrowKeys::LEFT :

        if (this->rgb_index == 0) {
            this->rgb_index = NUM_OF_COLORS - 1;
        } else {
            this->rgb_index--;
        }
        this->draw_values(this->rgb_index);

        break;
    default :
        this->send_bell();
        break;
    }


}

void NeopixelRgb::draw_reset(void) {

    this->green_val = 0;
    this->red_val = 0;
    this->blue_val = 0;

}

void NeopixelRgb::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
