/*
 * display_task.cpp
 *
 *  Created on: Jul 17, 2022
 *      Author: steph
 */

#include <console_uart.hpp>
#include "uartstdio.h"
#include "display_task.hpp"
#include "text_controls.hpp"

static const uint32_t MAX_DISPLAY_RATE_MS = 200;

DisplayUpdate::DisplayUpdate(void) {
    this->update_pending = false;
} // End DisplayUpdate::DisplayUpdate

bool DisplayUpdate::is_update_pending(void) {
    return this->update_pending;
} // End DisplayUpdate::is_update_pending

void DisplayUpdate::set_update_pending(bool update_pend) {
    this->update_pending = update_pend;
} // End DisplayUpdate::set_update_pending

void DisplayTask::taskfunwrapper(void* parm){
    (static_cast<DisplayTask*>(parm))->task((DisplayTask*)parm);
} // End DisplayTask::taskfunwrapper

DisplayTask::DisplayTask(void) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Display",            /* Text name for the task. */
                130,                  /* Stack size in words, not bytes. */
                this,                 /* Parameter passed into the task. */
                3,                    /* Priority at which the task is created. */
                NULL);

} // End DisplayTask::DisplayTask

void DisplayTask::add_display_update(DisplayUpdate* display_update) {

    this->display_updates.push_back(display_update);

} // End DisplayTask::add_display_update

void DisplayTask::init_display(void) {

} // DisplayTask::init_display

void DisplayTask::task(DisplayTask* this_ptr) {

    this->init_display();

    while(1){

        for (uint32_t idx=0; idx<display_updates.size(); idx++) {
            if (display_updates[idx]->is_update_pending()) {
                display_updates[idx]->update_display();
                display_updates[idx]->set_update_pending(false);
            }
        }

        vTaskDelay(MAX_DISPLAY_RATE_MS);
    }

} // End DisplayTask::task


