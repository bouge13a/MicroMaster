/*
 * one_wire_search.cpp
 *
 *  Created on: Aug 30, 2021
 *      Author: steph
 */

#include "one_wire_search.hpp"

#include "uartstdio.h"
#include "text_controls.hpp"


OneWireSearch::OneWireSearch(void) : ConsolePage("1 Wire Search",
                                            portMAX_DELAY,
                                            false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "1 wire search",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

} // End TestTask


void OneWireSearch::task(OneWireSearch* this_ptr) {

    while(1){


    }

} // End AdcTask::task

void OneWireSearch::draw_page(void) {

}
void OneWireSearch::draw_data(void) {

}
void OneWireSearch::draw_input(int character) {

}

void OneWireSearch::draw_reset(void) {

}

void OneWireSearch::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
