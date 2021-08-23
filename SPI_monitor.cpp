/*
 * SPI_monitor.cpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */


#include "SPI_monitor.hpp"

#include <uartstdio.h>
#include <utils.hpp>
#include "text_controls.hpp"

static const uint32_t START_ROW = 7;

static const uint32_t DATA_COL = 20;
static const uint32_t FORMAT_SPACING = 15;

static uint8_t bin_string[12];

static const char* format_types[] = {
    "HEX",
    "BIN",
    "MSB DEC",
    "LSB DEC",
};

void SpiMonitorTask::taskfunwrapper(void* parm){
    (static_cast<SpiMonitorTask*>(parm))->task((SpiMonitorTask*)parm);
} // End SpiMonitorTask::taskfunwrapper


SpiMonitorTask::SpiMonitorTask(SpiCmdTask* spi, std::vector<SpiMsg*>& spi_monitor_msgs) : ConsolePage("SPI Monitor",
                                       250,
                                       false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Spi Monitor",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                &this->task_handle);

    this->spi_monitor_msgs = spi_monitor_msgs;
    this->spi = spi;
    this->message_format = new spi_msg_format_e[NUM_OF_SPI_MONITORED_MSGS];
    this->format_index = 0;

    for (uint32_t index=0; index<NUM_OF_SPI_MONITORED_MSGS; index++) {
        this->message_format[index] = SPI_HEX_FORMAT;
    }

} // End I2cMonitorTask

