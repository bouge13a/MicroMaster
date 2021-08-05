/*
 * menu_page.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */


#include "menu_page.hpp"
#include "text_controls.hpp"
#include "uartstdio.h"

static const uint32_t START_ROW = 5;
static const uint32_t OPTION_SPACING = 15;
static const uint32_t OPTION_START_COL = 20;

MenuRow::MenuRow(uint32_t num_of_options,
                 void (*callback)(uint32_t index),
                 const char** options_names,
                 const char* row_name) {

    this->num_of_options = num_of_options;
    this->options_names = options_names;
    this->row_name = row_name;
    this->callback = callback;
    this->column_index = 0;

} // End MenuRow::MenuRow

MenuOptions::MenuOptions(void) : ConsolePage ("Configuration",
                                              portMAX_DELAY,
                                              false){

    this->menu_index = 0;

} // End MenuOptions::MenuOptions

void MenuOptions::add_menu_row(MenuRow* menu_row) {
    this->menu_rows.push_back(menu_row);
} // End MenuOptions::add_menu_row

void MenuOptions::print_row(uint32_t index) {

    for(uint32_t col_index=0; col_index<this->menu_rows[index]->num_of_options; col_index++) {

        TextCtl::cursor_pos(START_ROW + index, OPTION_START_COL + OPTION_SPACING*col_index);

        if (this->menu_index == index && col_index == this->menu_rows[index]->column_index) {
            TextCtl::text_color(TextCtl::blue_text);
            TextCtl::set_text_mode(TextCtl::mode_reverse);
        } else if (this->menu_rows[index]->column_index == col_index) {
            TextCtl::set_text_mode(TextCtl::mode_reverse);
        }

        UARTprintf("%s", this->menu_rows[index]->options_names[col_index]);

        if (this->menu_index == index && col_index == this->menu_rows[index]->column_index) {
            TextCtl::text_color(TextCtl::white_text);
            TextCtl::set_text_mode(TextCtl::mode_reverse_off);
        } else if (this->menu_rows[index]->column_index == col_index) {
            TextCtl::set_text_mode(TextCtl::mode_reverse_off);
        }

    }

} // End MenuOptions::print_row

void MenuOptions::draw_page(void) {

    for(uint32_t row_index=0; row_index<this->menu_rows.size(); row_index++) {

        TextCtl::cursor_pos(START_ROW + row_index, 0);
        UARTprintf("%s", this->menu_rows[row_index]->row_name);

        for(uint32_t col_index=0; col_index<this->menu_rows[row_index]->num_of_options; col_index++) {

            TextCtl::cursor_pos(START_ROW + row_index, OPTION_START_COL + OPTION_SPACING*col_index);

            if (this->menu_index == row_index && col_index == this->menu_rows[row_index]->column_index) {
                TextCtl::text_color(TextCtl::blue_text);
                TextCtl::set_text_mode(TextCtl::mode_reverse);
            } else if (this->menu_rows[row_index]->column_index == col_index) {
                TextCtl::set_text_mode(TextCtl::mode_reverse);
            }
            UARTprintf("%s", this->menu_rows[row_index]->options_names[col_index]);

            if (this->menu_index == row_index && col_index == this->menu_rows[row_index]->column_index) {
                TextCtl::text_color(TextCtl::white_text);
                TextCtl::set_text_mode(TextCtl::mode_reverse_off);
            } else if (this->menu_rows[row_index]->column_index == col_index) {
                TextCtl::set_text_mode(TextCtl::mode_reverse_off);
            }
        }
    }

} // End MenuOptions::draw_page

void MenuOptions::draw_data(void) {

} // End MenuOptions::draw_data

void MenuOptions::draw_input(int character) {

    uint32_t last_menu_index = this->menu_index;

    switch (character) {
    case ArrowKeys::DOWN :
        this->menu_index = (this->menu_index + 1) % this->menu_rows.size();
        this->print_row(last_menu_index);
        this->print_row(this->menu_index);
        break;
    case ArrowKeys::UP :
        if (0 == this->menu_index) {
            this->menu_index = this->menu_rows.size() - 1;
        } else {
            this->menu_index--;
        }
        this->print_row(last_menu_index);
        this->print_row(this->menu_index);
        break;
    case ArrowKeys::RIGHT :

        this->menu_rows[this->menu_index]->column_index =
                (this->menu_rows[this->menu_index]->column_index + 1) %
                 this->menu_rows[this->menu_index]->num_of_options;

        this->print_row(this->menu_index);
        this->menu_rows[this->menu_index]->callback(this->menu_rows[this->menu_index]->column_index);

        break;
    case ArrowKeys::LEFT :
        if (0 == this->menu_rows[this->menu_index]->column_index) {
            this->menu_rows[this->menu_index]->column_index = this->menu_rows[this->menu_index]->num_of_options - 1;
        } else {
            this->menu_rows[this->menu_index]->column_index--;
        }
        this->print_row(this->menu_index);
        this->menu_rows[this->menu_index]->callback(this->menu_rows[this->menu_index]->column_index);
        break;
    default :
        break;
    }

} // End MenuOptions::draw_input
