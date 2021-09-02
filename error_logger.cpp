/*
 * error_logger.cpp
 *
 *  Created on: Jul 29, 2021
 *      Author: steph
 */


#include "error_logger.hpp"
#include "text_controls.hpp"
#include "uartstdio.h"

static const uint32_t MAX_NUM_ERRORS = 40;
static const uint32_t START_ROW = 6;
static error_t errors[100];
static uint32_t num_of_errors = 0;

ErrorLogger* ErrorLogger::obj = NULL;

ErrorLogger::ErrorLogger(void) : ConsolePage("Error Logger",
                                             250,
                                             false) {



} // End ErrorLogger::ErrorLogger

ErrorLogger* ErrorLogger::get_instance(void) {
    if (obj == NULL) {
        obj = new ErrorLogger();
    }
    return obj;
}

error_t* ErrorLogger::create_error(const char* name,
                      const char* info) {

    if( num_of_errors + 1 > MAX_NUM_ERRORS) {
        assert(0);
    }

    errors[num_of_errors].name = name;
    errors[num_of_errors].info = info;
    errors[num_of_errors].occurences = 0;
    num_of_errors++;

    return &errors[num_of_errors-1];

} // End init_logger

void ErrorLogger::set_error(error_t* error) {

    error->occurences++;

} // End set_error

///////////////////////////////////////////////////////////
//                Console Page
//////////////////////////////////////////////////////////

static const uint32_t INFO_COL  = 17;
static const uint32_t OCCUR_COL = 60;

void ErrorLogger::draw_page(void){

    TextCtl::cursor_pos(START_ROW - 1, 0);
    UARTprintf("Press c to clear errors");

    TextCtl::cursor_pos(START_ROW , 0);
    TextCtl::set_text_mode(TextCtl::mode_underline);
    UARTprintf("NAME");

    TextCtl::cursor_pos(START_ROW , INFO_COL);
    UARTprintf("INFORMATION");

    TextCtl::cursor_pos(START_ROW , OCCUR_COL);
    UARTprintf("OCCURANCES");
    TextCtl::set_text_mode(TextCtl::mode_underline_off);

} // End logger_drawpage

void ErrorLogger::draw_data(void){

    uint32_t idx;
    uint32_t printed_errors = 0;

    TextCtl::text_color(TextCtl::bright_red_text);

    for (idx=0; idx < num_of_errors; idx++) {

        if (errors[idx].occurences > 0) {

            TextCtl::cursor_pos(START_ROW+1+printed_errors, 0);
            UARTprintf("%s", errors[idx].name);
            TextCtl::cursor_pos(START_ROW+1+printed_errors, INFO_COL);
            UARTprintf("%s", errors[idx].info);
            TextCtl::cursor_pos(START_ROW+1+printed_errors, OCCUR_COL);
            UARTprintf("%d", errors[idx].occurences);
            printed_errors++;

        }
    }

    TextCtl::text_color(TextCtl::white_text);

} // End logger_drawdata

void ErrorLogger::draw_input(int character){

    if ('c' == character) {
        for (uint32_t idx=0; idx < num_of_errors; idx++) {
            errors[idx].occurences = 0;

        }

        TextCtl::cursor_pos(START_ROW+1, 0);
        TextCtl::clear_below_line();
    }

} // End logger_drawinput

void ErrorLogger::draw_help(void){

} // End logger_drawinput

void ErrorLogger::draw_reset(void){

} // End logger_drawinput
