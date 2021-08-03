/*
 * I2C_monitor.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: steph
 */

#include <uartstdio.h>
#include "I2C_monitor.hpp"
#include "text_controls.hpp"

static const uint32_t START_ROW = 7;

static const uint32_t DATA_COL = 20;

void I2cMonitorTask::taskfunwrapper(void* parm){
    (static_cast<I2cMonitorTask*>(parm))->task((I2cMonitorTask*)parm);
} // End I2cMonitorTask::taskfunwrapper


I2cMonitorTask::I2cMonitorTask(I2cTask* i2c0, std::vector<I2cMsg*>& i2c_monitor_msgs) : ConsolePage("I2C Monitor",
                                       250,
                                       false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2c Monitor",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                &this->task_handle);

    this->i2c_monitor_msgs = i2c_monitor_msgs;
    this->i2c0 = i2c0;
} // End I2cMonitorTask


void I2cMonitorTask::task(I2cMonitorTask* this_ptr) {

    while(1){

        for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
            if (this->i2c_monitor_msgs[index]->monitored) {
                this->i2c0->add_i2c_msg(this->i2c_monitor_msgs[index]);
                vTaskDelay(1);
            }
        }

        vTaskDelay(200);

    }

} // End I2cMonitorTask

void I2cMonitorTask::draw_page(void) {

    TextCtl::cursor_pos(START_ROW, 0);

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
        UARTprintf("Message %d:\n\r", index + 1);
    }

}
void I2cMonitorTask::draw_data(void) {

    TextCtl::cursor_pos(START_ROW, DATA_COL);

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {

        if (this->i2c_monitor_msgs[index]->active) {
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            for(uint32_t inner_index = 0; inner_index<this->i2c_monitor_msgs[index]->bytes_rxed; inner_index++) {
                UARTprintf("0x%x ", this->i2c_monitor_msgs[index]->rx_data[inner_index]);
            }
        }
    }

}
void I2cMonitorTask::draw_input(int character) {

    if ('r' == character) {
        this->i2c0->reset_monitor_index();
        for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
            this->i2c_monitor_msgs[index]->active = false;
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            TextCtl::clear_in_line();
        }
    }

}
