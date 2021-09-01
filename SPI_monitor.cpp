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
                100,                  /* Stack size in words, not bytes. */
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

} // End SpiMonitorTask

void SpiMonitorTask::task(SpiMonitorTask* this_ptr) {

    while(1){

        for (uint32_t index=0; index<this_ptr->spi_monitor_msgs.size(); index++) {
            if (this_ptr->spi_monitor_msgs[index]->active) {
                this_ptr->spi->add_spi_msg(this_ptr->spi_monitor_msgs[index]);
                vTaskDelay(1);
            }
        }
        vTaskDelay(200);
    }
} // End SpiMonitorTask

void SpiMonitorTask::print_format_row(uint32_t row) {

    for (uint32_t index=0; index<SPI_NUM_OF_FORMATS; index++) {
        TextCtl::cursor_pos(START_ROW + this->spi_monitor_msgs.size() + 1 + row, DATA_COL + FORMAT_SPACING * index);
        if (index == this->message_format[row]) {
            TextCtl::set_text_mode(TextCtl::mode_reverse);
        }

        if ((this->format_index == row) && (index == this->message_format[this->format_index])) {
            TextCtl::text_color(TextCtl::blue_bgd);
        }
        UARTprintf("%s", format_types[index]);

        if (index == this->message_format[row]) {
            TextCtl::set_text_mode(TextCtl::mode_reverse_off);
        }

        if ((this->format_index == row) && (index == this->message_format[this->format_index])) {
            TextCtl::text_color(TextCtl::black_bgd);
        }
    }
} // End SpiMonitorTask::print_format_row

bool SpiMonitorTask::print_data(uint32_t inner_index, uint32_t index) {

    uint32_t data;

    switch(this->message_format[index]) {
    case SPI_HEX_FORMAT :
        UARTprintf("0x%2x ", this->spi_monitor_msgs[index]->rx_bytes[inner_index]);
        return false;
    case SPI_BIN_FORMAT :
        int_to_bin_string(this->spi_monitor_msgs[index]->rx_bytes[inner_index], bin_string);
        UARTprintf("0b%s ", bin_string);
        return false;
    case SPI_MSB_DEC_FORMAT :
        if (inner_index == 0) {
            if (this->spi_monitor_msgs[index]->num_rx_bytes == 1) {
                UARTprintf("%d", this->spi_monitor_msgs[index]->rx_bytes[0]);
            } else if (this->spi_monitor_msgs[index]->num_rx_bytes != 2) {
                UARTprintf("Not Applicable");
            } else {
                data = this->spi_monitor_msgs[index]->rx_bytes[0] << 4;
                data = data | this->spi_monitor_msgs[index]->rx_bytes[1];
                UARTprintf("%d", data);
            }
        }
        return true;
    case SPI_LSB_DEC_FORMAT :
        if (inner_index == 0) {
            if (this->spi_monitor_msgs[index]->num_rx_bytes == 1) {
                UARTprintf("%d", this->spi_monitor_msgs[index]->rx_bytes[0]);
            } else if (this->spi_monitor_msgs[index]->num_rx_bytes != 2) {
                UARTprintf("Not Applicable");
            } else {
                data = this->spi_monitor_msgs[index]->rx_bytes[0];
                data = data | (this->spi_monitor_msgs[index]->rx_bytes[1] << 8);
                UARTprintf("%d", data);
            }
        }
        return true;
    default :
        assert(0);
        return true;
    }

} // End SpiMonitorTask::print_data

void SpiMonitorTask::draw_page(void) {

    TextCtl::cursor_pos(START_ROW-2,0);
    UARTprintf("Press c to clear monitored messages");
    TextCtl::cursor_pos(START_ROW, 0);

    for (uint32_t index=0; index<this->spi_monitor_msgs.size(); index++) {
        UARTprintf("Message %d:\n\r", index + 1);
    }

    TextCtl::cursor_pos(START_ROW + spi_monitor_msgs.size() + 1, 0);

    for (uint32_t index=0; index<this->spi_monitor_msgs.size(); index++) {
        UARTprintf("Message Format %d:\n\r", index + 1);
    }

    for (uint32_t index=0; index<this->spi_monitor_msgs.size(); index++) {
        this->print_format_row(index);

    }


} // End I2cMonitorTask::draw_page

void SpiMonitorTask::draw_data(void) {

    TextCtl::cursor_pos(START_ROW, DATA_COL);

    for (uint32_t index=0; index<this->spi_monitor_msgs.size(); index++) {

        if (this->spi_monitor_msgs[index]->active) {
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            TextCtl::clear_in_line();
            if (0 == this->spi_monitor_msgs[index]->num_rx_bytes) {
                UARTprintf("No RX data");
            } else {
                for(uint32_t inner_index = 0; inner_index<this->spi_monitor_msgs[index]->bytes_rxed; inner_index++) {
                    this->print_data(inner_index, index);
                }
            }
        }
    }

} // End SpiMonitorTask::draw_data

void SpiMonitorTask::draw_input(int character) {

    uint32_t last_row;

    if(character == 'c') {
        this->spi->reset_monitor_index();
        for (uint32_t index=0; index<this->spi_monitor_msgs.size(); index++) {
            this->spi_monitor_msgs[index]->active = false;
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            TextCtl::clear_in_line();
        }
    }

    switch (character) {
    case ArrowKeys::DOWN :
        last_row = this->format_index;
        this->format_index = (format_index + 1) % NUM_OF_SPI_MONITORED_MSGS;
        this->print_format_row(last_row);
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::UP :
        last_row = this->format_index;
        if (this->format_index == 0) {
            this->format_index = NUM_OF_SPI_MONITORED_MSGS - 1;
        } else {
            this->format_index--;
        }
        this->print_format_row(last_row);
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::RIGHT :
        this->message_format[this->format_index] = (spi_msg_format_e)((this->message_format[this->format_index] + 1) % (SPI_NUM_OF_FORMATS));
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::LEFT :
        if(this->message_format[this->format_index] == 0) {
            this->message_format[this->format_index] = (spi_msg_format_e)(SPI_NUM_OF_FORMATS - 1);
        } else {
            this->message_format[this->format_index] = (spi_msg_format_e)(this->message_format[this->format_index] - 1);
        }
        this->print_format_row(this->format_index);
        break;
    default :
        break;
    }
} // End I2cMonitorTask::draw_input

void SpiMonitorTask::draw_reset(void) {

}

void SpiMonitorTask::draw_help(void) {

}

