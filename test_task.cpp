/*
 * test_class.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */

#include <console_uart.hpp>
#include "uartstdio.h"
#include "test_task.hpp"
#include "text_controls.hpp"

void TestTask::taskfunwrapper(void* parm){
    (static_cast<TestTask*>(parm))->task((TestTask*)parm);
} // End TestTask::taskfunwrapper


TestTask::TestTask(void) : ConsolePage("Test page",
                                       portMAX_DELAY,
                                       false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Test",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

} // End TestTask


void TestTask::task(TestTask* this_ptr) {

    while(1){

        this_ptr->test = 10;

        vTaskDelay(1000);

    }

} // End AdcTask::task

void TestTask::draw_page(void) {

    UARTprintf("THIS IS ONLY A TEST\r\n");
    UARTprintf("THIS IS ONLY A TEST\r\n");
    UARTprintf("THIS IS ONLY A TEST\r\n");

}
void TestTask::draw_data(void) {

}
void TestTask::draw_input(int character) {

}

void TestTask::draw_reset(void) {

}

void TestTask::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
