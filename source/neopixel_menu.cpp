/*
 * neopixel_menu.cpp
 *
 *  Created on: Oct 21, 2021
 *      Author: steph
 */


#include "neopixel_menu.hpp"

#include <console_uart.hpp>
#include "uartstdio.h"
#include "test_task.hpp"
#include "text_controls.hpp"
#include "neopixel_constants.hpp"

#include <cstdlib>


static volatile uint32_t num_of_leds = 0;
static volatile uint32_t brightness = 0;
static volatile uint32_t speed = 0;

static const uint32_t MENU_START_ROW = 6;
static const uint32_t OPTIONS_START_COL = 15;
static const uint32_t SIZE_OF_MENU_BAR = 50;

static const uint32_t MAX_NEOPIXELS = 999;

static bool neopixel_on = true;

static const int32_t RAINBOW_INCR = 1;

static const uint32_t WHITE  = 0x00ffffff;
static const uint32_t GREEN  = 0x00ff0000;
static const uint32_t RED    = 0x0000ff00;
static const uint32_t BLUE   = 0x000000ff;
static const uint32_t ORANGE = 0x00ffff00;
static const uint32_t CYAN   = 0x00ff00ff;
static const uint32_t PURPLE = 0x0000ffff;

static const char* options_rows[] = {
                                     "Off",
                                     "Solid",
                                     "Chaos",
                                     "Stream",
                                     "Mood",
                                     "Rainbow",
                                     "Strobe",
};

static const uint32_t NUM_OF_OPTIONS = 7;

static const char* width_rows[] = {
                                 "rgb",
                                 "rgbw",
};

static const uint32_t NUM_OF_WIDTHS = 2;

static NeopixelCtl* neopix_command = NULL;


neopix_modes_e mode = OFF_MODE;

static void width_row(uint32_t index, neopix_dir_e direction, bool selected) {

    for(uint32_t loop_index=0; loop_index<NUM_OF_WIDTHS; loop_index++) {
        if (loop_index == index) {

            if(selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }

            UARTprintf("%s", width_rows[loop_index]);
            TextCtl::bgd_color(TextCtl::black_bgd);
            UARTprintf("     ");

        } else {
            UARTprintf("%s     ", width_rows[loop_index]);
        }
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            neopix_command->set_width(32);
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            neopix_command->set_width(24);
        }
    }

}

static void first_led_digit_row(uint32_t index, neopix_dir_e direction, bool selected) {

    for(uint32_t loop_idx=0; loop_idx<10; loop_idx++) {
        if (loop_idx == index) {

            if (selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }
            UARTprintf("%d", loop_idx);
            TextCtl::bgd_color(TextCtl::black_bgd);
        } else {
            UARTprintf("%d", loop_idx);
        }

        UARTprintf("    ");
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            num_of_leds += 1;
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            num_of_leds -= 1;
        }
    }
}

static void second_led_digit_row(uint32_t index, neopix_dir_e direction, bool selected) {

    for(uint32_t loop_idx=0; loop_idx<10; loop_idx++) {
        if (loop_idx == index) {

            if (selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }
            UARTprintf("%d", loop_idx);
            TextCtl::bgd_color(TextCtl::black_bgd);
        } else {
            UARTprintf("%d", loop_idx);
        }

        UARTprintf("    ");
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            num_of_leds += 10;
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            num_of_leds -= 10;
        }
    }
}

static void third_led_digit_row(uint32_t index, neopix_dir_e direction, bool selected) {

    for(uint32_t loop_idx=0; loop_idx<10; loop_idx++) {
        if (loop_idx == index) {

            if (selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }
            UARTprintf("%d", loop_idx);
            TextCtl::bgd_color(TextCtl::black_bgd);
        } else {
            UARTprintf("%d", loop_idx);
        }

        UARTprintf("    ");
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            num_of_leds += 100;
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            num_of_leds -= 100;
        }
    }
}

static void brightness_bar(uint32_t index, neopix_dir_e direction, bool selected) {

    TextCtl::bgd_color(TextCtl::white_bgd);
    for(uint32_t loop_index=0; loop_index<SIZE_OF_MENU_BAR; loop_index++) {
        if (loop_index == index) {

            if(selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }

            UARTprintf(" ");

            TextCtl::bgd_color(TextCtl::white_bgd);
        } else {
            UARTprintf(" ");
        }
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            brightness += 2;
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            brightness -= 2;
        }
    }

    TextCtl::bgd_color(TextCtl::black_bgd);
}

static void speed_bar(uint32_t index, neopix_dir_e direction, bool selected) {

    TextCtl::bgd_color(TextCtl::white_bgd);
    for(uint32_t loop_index=0; loop_index<SIZE_OF_MENU_BAR; loop_index++) {
        if (loop_index == index) {

            if(selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }

            UARTprintf(" ");

            TextCtl::bgd_color(TextCtl::white_bgd);
        } else {
            UARTprintf(" ");
        }
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            speed += 2;
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            speed -= 2;
        }
    }

    TextCtl::bgd_color(TextCtl::black_bgd);
}

static void modes_row(uint32_t index, neopix_dir_e direction, bool selected) {

    for(uint32_t loop_index=0; loop_index<NUM_OF_OPTIONS; loop_index++) {
        if (loop_index == index) {

            if(selected) {
                TextCtl::bgd_color(TextCtl::blue_bgd);
            } else {
                TextCtl::bgd_color(TextCtl::cyan_bgd);
            }

            UARTprintf("%s", options_rows[loop_index]);
            TextCtl::bgd_color(TextCtl::black_bgd);
            UARTprintf("    ");

        } else {
            UARTprintf("%s    ", options_rows[loop_index]);
        }
    }

    if (selected){
        if (direction == NEOPIX_RIGHT_DIR) {
            mode = (neopix_modes_e)(mode + 1);
        } else if ( direction == NEOPIX_LEFT_DIR ) {
            mode = (neopix_modes_e)(mode - 1);
        }
    }

    neopixel_on = true;
}

NeopixRow::NeopixRow(void (*callback)(uint32_t index, neopix_dir_e direction, bool selected),
                     uint32_t num_of_items,
                     const char* name) {

    this->callback = callback;
    this->num_of_items = num_of_items;
    this->menu_index = 0;
    this->name = name;

}

void NeopixelMenu::taskfunwrapper(void* parm){
    (static_cast<NeopixelMenu*>(parm))->task((NeopixelMenu*)parm);
} // End NeopixelMenu::taskfunwrapper


NeopixelMenu::NeopixelMenu(NeopixelCtl* neopix_cmd) : ConsolePage("Neopixel Menu",
                                                                  portMAX_DELAY,
                                                                  false) {

    this->neopix_cmd = neopix_cmd;
    neopix_command = neopix_cmd;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Test",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->menu_rows.push_back(new NeopixRow(width_row,
                                            NUM_OF_WIDTHS,
                                            "Type"));

    this->menu_rows.push_back(new NeopixRow(third_led_digit_row,
                                            10,
                                            "100's Place"));

    this->menu_rows.push_back(new NeopixRow(second_led_digit_row,
                                            10,
                                            "10's Place"));

    this->menu_rows.push_back(new NeopixRow(first_led_digit_row,
                                            10,
                                            "1's Place"));

    this->menu_rows.push_back(new NeopixRow(brightness_bar,
                                            SIZE_OF_MENU_BAR,
                                            "Brightness"));

    this->menu_rows.push_back(new NeopixRow(speed_bar,
                                            SIZE_OF_MENU_BAR,
                                            "Speed"));

    this->menu_rows.push_back(new NeopixRow(modes_row,
                                            NUM_OF_OPTIONS,
                                            "Modes"));

    this->colors.push_back(WHITE);
    this->colors.push_back(GREEN);
    this->colors.push_back(RED);
    this->colors.push_back(BLUE);
    this->colors.push_back(ORANGE);
    this->colors.push_back(CYAN);
    this->colors.push_back(PURPLE);

    this->vert_menu_index = 0;
    this->solid_mode_counter = 0;
    this->rainbow_counter = 0;
    this->brightness_counter = 0;
    this->brightness_counter_dir = NEOPIX_RIGHT_DIR;
    this->rainbow_state = RAINBOW_START;

    this->neopix_values = new uint32_t[MAX_NEOPIXELS];
    this->brightness_values = new uint8_t[MAX_NEOPIXELS];

    for(uint32_t index=0; index<MAX_NEOPIXELS; index++) {
        this->brightness_values[index] = 0;
    }

    this->neopix_msg = new NeopixMsg(neopix_normal_msg);
    this->neopix_msg->tx_msgs = this->neopix_values;
    this->neopix_msg->msg_state = neopix_idle;

    this->neopix_stream_msg = new NeopixMsg(neopix_streamer_msg);
    this->neopix_stream_msg->tx_msgs = this->neopix_values;
    this->neopix_stream_msg->msg_state = neopix_idle;

    this->neopix_clr_msg = new NeopixMsg(neopix_clear_msg);
    this->neopix_clr_msg->tx_msgs = this->neopix_values;
    this->neopix_clr_msg->msg_state = neopix_idle;

    this->neopix_rainbow_msg = new NeopixMsg(neopixel_rainbow_msg);
    this->neopix_rainbow_msg->tx_msgs = this->neopix_values;
    this->neopix_rainbow_msg->msg_state = neopix_idle;

} // End NeopixelMenu


void NeopixelMenu::task(NeopixelMenu* this_ptr) {

    while(1){

        switch(mode) {
        case OFF_MODE :

            if (neopixel_on) {
                this_ptr->neopix_clr_msg->num_tx_msgs = num_of_leds;
                this_ptr->neopix_cmd->add_msg(this_ptr->neopix_clr_msg);
                neopixel_on = false;
                this->rainbow_state = RAINBOW_START;
            }

            break;

        case SOLID_BLINK_MODE :

            for(uint32_t index=0; index<num_of_leds; index++) {
                this_ptr->neopix_msg->tx_msgs[index] = this_ptr->colors[this_ptr->solid_mode_counter];
                this_ptr->change_brightness(&this_ptr->neopix_msg->tx_msgs[index], brightness);
            }

            this_ptr->solid_mode_counter = (this->solid_mode_counter+1) %  this_ptr->colors.size();

            this_ptr->neopix_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_cmd->add_msg(this_ptr->neopix_msg);

            vTaskDelay(1010 - 10*speed);

            break;

        case CHAOS_MODE :

            for(uint32_t index=0; index<num_of_leds; index++) {
                this_ptr->neopix_msg->tx_msgs[index] = this_ptr->colors[rand() % this_ptr->colors.size()];
                this_ptr->change_brightness(&this_ptr->neopix_msg->tx_msgs[index], brightness);
            }

            this_ptr->neopix_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_cmd->add_msg(this_ptr->neopix_msg);

            vTaskDelay(1010 - 10*speed);

            break;

        case STREAM_MODE :

            for(uint32_t index=0; index<num_of_leds; index++) {


                this_ptr->neopix_stream_msg->tx_msgs[index] = rainbow_array[index%30];

                this_ptr->change_brightness(&this_ptr->neopix_stream_msg->tx_msgs[index], brightness);

            }

            this_ptr->neopix_stream_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_cmd->add_msg(this_ptr->neopix_stream_msg);

            vTaskDelay(1010 - 10*speed);


            break;

        case MOOD_MODE :

            if (this_ptr->brightness_counter == 0) {
                this_ptr->solid_mode_counter = (this->solid_mode_counter+1) %  this_ptr->colors.size();
            }

            for(uint32_t index=0; index<num_of_leds; index++) {
                this_ptr->neopix_msg->tx_msgs[index] = this_ptr->colors[this_ptr->solid_mode_counter];
            }

            if (this_ptr->brightness_counter_dir == NEOPIX_RIGHT_DIR) {
                this_ptr->brightness_counter++;
                if (this_ptr->brightness_counter >= brightness) {
                    this_ptr->brightness_counter_dir = NEOPIX_LEFT_DIR;
                }
            } else if (this_ptr->brightness_counter_dir == NEOPIX_LEFT_DIR) {
                this_ptr->brightness_counter--;
                if (this_ptr->brightness_counter == 0) {
                    this_ptr->brightness_counter_dir = NEOPIX_RIGHT_DIR;
                }
            }

            for(uint32_t index=0; index<num_of_leds; index++) {
                this_ptr->change_brightness(&this_ptr->neopix_msg->tx_msgs[index], this_ptr->brightness_counter);
            }

            this_ptr->neopix_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_cmd->add_msg(this_ptr->neopix_msg);

            vTaskDelay(1010 - 10*speed);

            break;

        case RAINBOW_MODE :

            if (this_ptr->rainbow_state == RAINBOW_START) {

                for(uint32_t index = 0; index<num_of_leds; index++) {
                    this_ptr->neopix_msg->tx_msgs[index] = WHITE;
                    this_ptr->change_brightness(&this_ptr->neopix_stream_msg->tx_msgs[index], brightness);
                }

                this_ptr->rainbow_state = RAINBOW_TO_GREEN;

            } else if (this_ptr->rainbow_state == RAINBOW_TO_GREEN) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_BLUE, -RAINBOW_INCR);
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_RED, -RAINBOW_INCR);
                    }
                } else {

                    this_ptr->rainbow_state = RAINBOW_YELLOW;
                    this_ptr->rainbow_counter = 0;
                }

            }  else if (this_ptr->rainbow_state == RAINBOW_YELLOW) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_RED, RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_RED;
                    this_ptr->rainbow_counter=0;
                }
            } else if (this_ptr->rainbow_state == RAINBOW_RED) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_GREEN, -RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_PURPLE;
                    this_ptr->rainbow_counter=0;
                }
            } else if (this_ptr->rainbow_state == RAINBOW_PURPLE) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_BLUE, RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_BLUE;
                    this_ptr->rainbow_counter=0;
                }
            } else if (this_ptr->rainbow_state == RAINBOW_BLUE) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_RED, -RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_CYAN;
                    this_ptr->rainbow_counter=0;
                }
            } else if (this_ptr->rainbow_state == RAINBOW_CYAN) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_GREEN, RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_FADE;
                    this_ptr->rainbow_counter=0;
                }
            } else if (this_ptr->rainbow_state == RAINBOW_FADE) {

                if (this_ptr->rainbow_counter + RAINBOW_INCR < (brightness/100.0)*0xff ) {
                    this_ptr->rainbow_counter = this_ptr->rainbow_counter + RAINBOW_INCR;
                    for (uint32_t index=0; index<num_of_leds; index++) {
                        this_ptr->incr_color_brightness(&this_ptr->neopix_msg->tx_msgs[index], NEOPIX_RED, RAINBOW_INCR);
                    }
                } else {
                    this->rainbow_state = RAINBOW_START;
                    this_ptr->rainbow_counter=0;
                }
            }

            this_ptr->neopix_rainbow_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_cmd->add_msg(this_ptr->neopix_rainbow_msg);

            vTaskDelay(1010 - 10*speed);

            break;

        case STROBE_MODE :

            for(uint32_t index=0; index<num_of_leds; index++) {
                this_ptr->neopix_msg->tx_msgs[index] = WHITE;
                this_ptr->change_brightness(&this_ptr->neopix_msg->tx_msgs[index], brightness);
            }

            this_ptr->solid_mode_counter = (this->solid_mode_counter+1);

            this_ptr->neopix_msg->num_tx_msgs = num_of_leds;
            this_ptr->neopix_clr_msg->num_tx_msgs = num_of_leds;

            if (this->solid_mode_counter % 2) {
                this_ptr->neopix_cmd->add_msg(this_ptr->neopix_msg);
            } else {
                this_ptr->neopix_cmd->add_msg(this_ptr->neopix_clr_msg);
            }

            vTaskDelay(1010 - 10*speed);

            break;

        default :
            assert(0);
            break;



        }



        vTaskDelay(1);

    }

} // End NeopixelMenu::task

void NeopixelMenu::change_brightness(uint32_t* value, uint32_t brightness) {

    float bright_mult = brightness/100.0;

    uint32_t green_val;
    uint32_t red_val;
    uint32_t blue_val;

    green_val = (uint32_t)((uint32_t)((0x00ff0000 & *value) >> 16) * bright_mult) << 16;
    red_val   = (uint32_t)((uint32_t)((0x0000ff00 & *value) >> 8) * bright_mult) << 8;
    blue_val  = (uint32_t)((uint32_t)((0x000000ff & *value)) * bright_mult);


    *value =  green_val | red_val | blue_val;

    return;
}

void NeopixelMenu::incr_color_brightness(uint32_t* value,
                                        neopix_colors_e color,
                                        int32_t incr) {


    uint32_t green_val = (0x00ff0000 & *value) >> 16;
    uint32_t red_val = (0x0000ff00 & *value) >> 8;
    uint32_t blue_val = (0x000000ff & *value);

    switch(color) {
    case NEOPIX_GREEN :

        if (incr > 0) {
            if (green_val + incr < 0xff) {
                green_val += incr;
            }
        } else {
            if (green_val - incr > 0 ) {
                green_val += incr;
            }
        }

        break;
    case NEOPIX_RED :

        if (incr > 0) {
            if (red_val + incr < 0xff) {
                red_val += incr;
            }
        } else {
            if (red_val - incr > 0 ) {
                red_val += incr;
            }
        }

        break;
    case NEOPIX_BLUE :

        if (incr > 0) {
            if (blue_val + incr < 0xff) {
                blue_val += incr;
            }
        } else {
            if (blue_val - incr > 0 ) {
                blue_val += incr;
            }
        }
        break;
    default :
        assert(0);
        break;

    }

    *value =  (green_val << 16) | (red_val << 8) | blue_val;

    return;

} // End NeopixelMenu::incr_color_brightness

void NeopixelMenu::draw_page(void) {

    for (uint32_t index=0; index<this->menu_rows.size(); index++) {

        TextCtl::cursor_pos(MENU_START_ROW + 2*index, 0);
        UARTprintf("%s", this->menu_rows[index]->name);
        TextCtl::cursor_pos(MENU_START_ROW + 2*index, OPTIONS_START_COL);
        this->menu_rows[index]->callback(this->menu_rows[index]->menu_index, NEOPIX_NO_DIR, index==this->vert_menu_index ? true : false );
    }


}
void NeopixelMenu::draw_data(void) {

}
void NeopixelMenu::draw_input(int character) {


    switch (character) {
    case ArrowKeys::DOWN :


        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();
        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_NO_DIR,
                                                         false);


        this->vert_menu_index = (this->vert_menu_index + 1) % this->menu_rows.size();

        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();

        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_NO_DIR,
                                                         true);

        break;
    case ArrowKeys::UP :

        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();

        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_NO_DIR,
                                                         false);

        if (this->vert_menu_index == 0) {
            this->vert_menu_index = this->menu_rows.size() - 1;
        } else {
            this->vert_menu_index--;
        }

        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();

        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_NO_DIR,
                                                         true);

        break;
    case ArrowKeys::RIGHT :

        if(this->menu_rows[this->vert_menu_index]->menu_index == this->menu_rows[this->vert_menu_index]->num_of_items - 1) {
           break;
        }

        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();

        this->menu_rows[this->vert_menu_index]->menu_index++;

        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_RIGHT_DIR,
                                                         true);
        break;
    case ArrowKeys::LEFT :

        if(this->menu_rows[this->vert_menu_index]->menu_index == 0) {
           break;
        }

        TextCtl::cursor_pos(MENU_START_ROW + this->vert_menu_index*2, OPTIONS_START_COL);
        TextCtl::clear_in_line();


        this->menu_rows[this->vert_menu_index]->menu_index--;


        this->menu_rows[this->vert_menu_index]->callback(this->menu_rows[this->vert_menu_index]->menu_index,
                                                         NEOPIX_LEFT_DIR,
                                                         true);

        break;
    default :
        this->send_bell();
        break;
    }




}

void NeopixelMenu::draw_reset(void) {

}

void NeopixelMenu::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
