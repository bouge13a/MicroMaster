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
#include <assert.h>

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

DisplayTask::DisplayTask(OLED_GFX* oled_gfx,
                         SemaphoreHandle_t display_sem,
                         uint32_t* power_idx) {
    this->power_idx = power_idx;
    this->display_sem = display_sem;
    this->oled_gfx = oled_gfx;
    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Display",            /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                 /* Parameter passed into the task. */
                3,                    /* Priority at which the task is created. */
                NULL);


} // End DisplayTask::DisplayTask

void DisplayTask::set_suite(suite_e suite) {
    this->suite = suite;
} // End DisplayTask::set_suite

void DisplayTask::add_display_update(DisplayUpdate* display_update) {

    this->display_updates.push_back(display_update);

} // End DisplayTask::add_display_update

void DisplayTask::task(DisplayTask* this_ptr) {

    this_ptr->oled_gfx->init();

    this_ptr->oled_gfx->oled->send_str_xy("MicroMaster", 4, 4);

    xSemaphoreTake(this_ptr->display_sem, portMAX_DELAY);

    this_ptr->oled_gfx->oled->reset_display();

    switch(this_ptr->suite) {
    case MAIN_SUITE :
        this_ptr->draw_main_suite();
        break;
    case FTDI_SUITE :
        break;
    case NEOPIXEL_SUITE:
        break;
    case PWM_SUITE:
        break;
    default :
        assert(0);
    }

    while(1){

        for (uint32_t idx=0; idx<this_ptr->display_updates.size(); idx++) {
            if (this_ptr->display_updates[idx]->is_update_pending()) {
                this_ptr->display_updates[idx]->update_display();
                this_ptr->display_updates[idx]->set_update_pending(false);
            }
        }

        vTaskDelay(MAX_DISPLAY_RATE_MS);
    }

} // End DisplayTask::task

void DisplayTask::draw_main_suite(void) {
    this->oled_gfx->oled->send_str_xy("PSU(State)", 0, 0);
    this->oled_gfx->oled->send_str_xy("PSU(V)", 1, 0);
    this->oled_gfx->oled->send_str_xy("I(mA)", 2, 0);
    this->oled_gfx->oled->send_str_xy("I2C PU", 3, 0);
    this->oled_gfx->oled->send_str_xy("Errors", 4, 0);

    this->oled_gfx->oled->send_str_xy("Off", 0, 12);
    this->oled_gfx->oled->send_str_xy("On ", 3, 12);

    switch (*this->power_idx) {
    case 0:
        this->oled_gfx->oled->send_str_xy("3.3", 1, 12);
        break;
    case 1:
        this->oled_gfx->oled->send_str_xy("5.0", 1, 12);
        break;
    case 3:
        this->oled_gfx->oled->send_str_xy("2.8", 1, 12);
        break;
    default:
        break;
    }
}
