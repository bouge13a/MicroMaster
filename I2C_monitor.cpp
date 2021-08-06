/*
 * I2C_monitor.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: steph
 */

#include <uartstdio.h>
#include <utils.hpp>
#include "I2C_monitor.hpp"
#include "text_controls.hpp"

static const uint32_t START_ROW = 7;

static const uint32_t DATA_COL = 20;
static const uint32_t FORMAT_SPACING = 15;

uint8_t bin_string[12];

static const char* format_types[] = {
    "HEX",
    "BIN",
    "MSB DEC",
    "LSB DEC",
};

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
    this->message_format = new msg_format_e[NUM_OF_MONITORED_MSGS];
    this->format_index = 0;

    for (uint32_t index=0; index<NUM_OF_MONITORED_MSGS; index++) {
        this->message_format[index] = HEX_FORMAT;
    }

} // End I2cMonitorTask


void I2cMonitorTask::task(I2cMonitorTask* this_ptr) {

    while(1){

        for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
            if (this->i2c_monitor_msgs[index]->active) {
                this->i2c0->add_i2c_msg(this->i2c_monitor_msgs[index]);
                vTaskDelay(1);
            }
        }

        vTaskDelay(200);

    }

} // End I2cMonitorTask

void I2cMonitorTask::print_format_row(uint32_t row) {

    for (uint32_t index=0; index<NUM_OF_FORMATS; index++) {
        TextCtl::cursor_pos(START_ROW + i2c_monitor_msgs.size() + 1 + row, DATA_COL + FORMAT_SPACING * index);
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
} // End I2cMonitorTask::print_format_row

bool I2cMonitorTask::print_data(uint32_t inner_index, uint32_t index) {

    uint32_t data;

    switch(this->message_format[index]) {
    case HEX_FORMAT :
        UARTprintf("0x%x ", this->i2c_monitor_msgs[index]->rx_data[inner_index]);
        return false;
    case BIN_FORMAT :
        int_to_bin_string(this->i2c_monitor_msgs[index]->rx_data[inner_index], bin_string);
        UARTprintf("0b%s ", bin_string);
        return false;
    case MSB_DEC_FORMAT :
        if (this->i2c_monitor_msgs[index]->num_rx_bytes != 2) {
            UARTprintf("Not Applicable");
        } else {
            data = this->i2c_monitor_msgs[index]->rx_data[0] << 4;
            data = data | this->i2c_monitor_msgs[index]->rx_data[1];
            UARTprintf("%d", data);
        }
        return true;
    case LSB_DEC_FORMAT :
        if (this->i2c_monitor_msgs[index]->num_rx_bytes != 2) {
            UARTprintf("Not Applicable");
        } else {
            data = this->i2c_monitor_msgs[index]->rx_data[0];
            data = data | (this->i2c_monitor_msgs[index]->rx_data[1] << 8);
            UARTprintf("%d", data);
        }
        return true;
    default :
        assert(0);
        return true;
    }

} // End I2cMonitorTask::print_data

void I2cMonitorTask::draw_page(void) {

    TextCtl::cursor_pos(START_ROW-2,0);
    UARTprintf("Press c to clear monitored messages");
    TextCtl::cursor_pos(START_ROW, 0);

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
        UARTprintf("Message %d:\n\r", index + 1);
    }

    TextCtl::cursor_pos(START_ROW + i2c_monitor_msgs.size() + 1, 0);

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
        UARTprintf("Message Format %d:\n\r", index + 1);
    }

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
        this->print_format_row(index);

    }


} // End I2cMonitorTask::draw_page

void I2cMonitorTask::draw_data(void) {

    TextCtl::cursor_pos(START_ROW, DATA_COL);

    for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {

        if (this->i2c_monitor_msgs[index]->active) {
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            TextCtl::clear_in_line();
            for(uint32_t inner_index = 0; inner_index<this->i2c_monitor_msgs[index]->bytes_rxed; inner_index++) {
                if(this->print_data(inner_index, index)) {
                    break;
                }
            }
        }
    }

} // End I2cMonitorTask::draw_data

void I2cMonitorTask::draw_input(int character) {

    uint32_t last_row;

    if(character == 'c') {
        this->i2c0->reset_monitor_index();
        for (uint32_t index=0; index<this->i2c_monitor_msgs.size(); index++) {
            this->i2c_monitor_msgs[index]->active = false;
            TextCtl::cursor_pos(START_ROW + index, DATA_COL);
            TextCtl::clear_in_line();
        }
    }

    switch (character) {
    case ArrowKeys::DOWN :
        last_row = this->format_index;
        this->format_index = (format_index + 1) % NUM_OF_MONITORED_MSGS;
        this->print_format_row(last_row);
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::UP :
        last_row = this->format_index;
        if (this->format_index == 0) {
            this->format_index = NUM_OF_MONITORED_MSGS - 1;
        } else {
            this->format_index--;
        }
        this->print_format_row(last_row);
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::RIGHT :
        this->message_format[this->format_index] = (msg_format_e)((this->message_format[this->format_index] + 1) % (NUM_OF_FORMATS));
        this->print_format_row(this->format_index);
        break;
    case ArrowKeys::LEFT :
        if(this->message_format[this->format_index] == 0) {
            this->message_format[this->format_index] = (msg_format_e)(NUM_OF_FORMATS - 1);
        } else {
            this->message_format[this->format_index] = (msg_format_e)(this->message_format[this->format_index] - 1);
        }
        this->print_format_row(this->format_index);
        break;
    default :
        break;
    }
} // End I2cMonitorTask::draw_input

void I2cMonitorTask::draw_reset(void) {



}

void I2cMonitorTask::draw_help(void) {

}

