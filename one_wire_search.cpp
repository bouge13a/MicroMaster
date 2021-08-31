/*
 * one_wire_search.cpp
 *
 *  Created on: Aug 30, 2021
 *      Author: steph
 */

#include "one_wire_search.hpp"

#include "uartstdio.h"
#include "text_controls.hpp"

static SemaphoreHandle_t timer_semphr = NULL;

OneWireSearch::OneWireSearch(GpoObj* gpo_obj) : ConsolePage("1 Wire Search",
                                                            portMAX_DELAY,
                                                            false) {
    this->gpo_obj = gpo_obj;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "1 Wire Search",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    timer_semphr = xSemaphoreCreateBinary();

    this->one_wire_q = xQueueCreate(2, sizeof(uint32_t*));



} // End TestTask


void OneWireSearch::task(OneWireSearch* this_ptr) {

    while(1){

        vTaskDelay(1000);
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
