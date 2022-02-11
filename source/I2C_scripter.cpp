/*
 * I2C_scripter.cpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */

#include "I2C_scripter.hpp"


#include "text_controls.hpp"
#include "uartstdio.h"
#include <assert.h>

static const uint32_t TX_BUFFER_SIZE = 50;
static const uint32_t START_ROW = 5;


void I2cScripterTask::taskfunwrapper(void* parm){
    (static_cast<I2cScripterTask*>(parm))->task((I2cScripterTask*)parm);
} // End I2cScripterTask::taskfunwrapper


I2cScripterTask::I2cScripterTask(I2cTask* i2c) : ConsolePage("I2C Scripter",
                                                            portMAX_DELAY,
                                                            false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C script",         /* Text name for the task. */
                80,                  /* Stack size in words, not bytes. */
                this,                 /* Parameter passed into the task. */
                3,                    /* Priority at which the task is created. */
                NULL);

    this->i2c = i2c;

    this-> send_semphr = xSemaphoreCreateBinary();

    this->i2c_msg = new I2cMsg(normal_msg);
    this->i2c_msg->tx_data = new uint8_t[TX_BUFFER_SIZE];
    this->i2c_msg->num_rx_bytes = 0;

    this->buffer_idx = 0;

} // End I2cScripterTask::I2cScripterTask


void I2cScripterTask::task(I2cScripterTask* this_ptr) {

    while(1){

        xSemaphoreTake(this->send_semphr, portMAX_DELAY);

        this->i2c->add_i2c_msg(i2c_msg);

        while (this->i2c_msg->state != i2c_finished) {
            vTaskDelay(1);
        }

        this->i2c_msg->state = i2c_ready;

        if (this->on_screen) {
            this->print_message(this->i2c_msg);
            UARTprintf("\r\nTransmit messages in the format [xx xx xx ...] starting with address\r\n");

        }

    }

} // End I2cScripterTask::task

void I2cScripterTask::print_message(I2cMsg* i2c_msg) {

    switch (this->i2c_msg->errors) {
    case NONE :
        UARTprintf("Message transmitted successfully\r\n");
        break;
    case ADDR_NACK_ERR :
        UARTprintf("Error: Address NACK\r\n");
        break;
    case DATA_NACK_ERR :
        UARTprintf("Error: Data NACK\r\n");
        break;
    case ARB_LOST_ERR :
        UARTprintf("Error: Arbitration lost\r\n");
        break;
    case TIMEOUT_ERR :
        UARTprintf("Error: Timeout\r\n");
        break;
    case PULL_UP_ERR :
        UARTprintf("Error: I2c lines low, check pull up resistors\r\n");
        break;
    default :
        assert(0);
        break;
    }

} // End I2cScripterTask::print_message

bool I2cScripterTask::ascii_to_hex(char* character) {

    if (*character - '0' <=  9) {
        UARTprintf("%c", *character);
        *character = *character - '0';

        return true;
    } else if ((*character - 'a' >= 0) && (*character - 'a' < 6)) {
        UARTprintf("%c", *character);
        *character = *character - 'a' + 10;

        return true;
    } else {
        return false;
    }

} // End I2cScripterTask::ascii_to_hex

void I2cScripterTask::draw_page(void) {

    TextCtl::cursor_pos(START_ROW, 0);
    UARTprintf("Press r to reset\r\n");
    UARTprintf("Transmit messages in the format [xx xx xx ...] starting with address\r\n");

} // End I2cScripterTask::draw_page

void I2cScripterTask::draw_data(void) {

} // End I2cScripterTask::draw_data

void I2cScripterTask::draw_input(int character) {

    switch(this->buffer_state) {
    case START_SCRIPTER :

        if ('[' == character) {
            this->buffer_state = GET_NIBBLE_0;
            UARTprintf("%c", character);
            this->i2c_msg->num_tx_bytes = 0;
        } else {
            this->send_bell();
        }

        break;

    case GET_NIBBLE_0 :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (0 == this->buffer_idx) {

                if (this->ascii_to_hex((char*)&character)) {

                    this->i2c_msg->address = character << 4;
                    this->buffer_state = GET_NIBBLE_1;
                }

            } else if (this->buffer_idx < TX_BUFFER_SIZE) {

                if (this->ascii_to_hex((char*)&character)) {

                    this->i2c_msg->tx_data[this->i2c_msg->num_tx_bytes] = character << 4;

                    this->buffer_state = GET_NIBBLE_1;
                }

            } else {

                UARTprintf("\r\nError: too many bytes\r\n");
                this->buffer_state = START_SCRIPTER;
                this->buffer_idx = 0;

            }
        } else {
            this->send_bell();
        }

        break;

    case GET_NIBBLE_1 :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){


            if (0 == this->buffer_idx) {

                if (this->ascii_to_hex((char*)&character)) {

                    this->i2c_msg->address = this->i2c_msg->address | character;

                    this->buffer_state = GET_SPACE_BAR;
                    this->buffer_idx++;
                }

            } else if (this->buffer_idx < TX_BUFFER_SIZE) {

                if (this->ascii_to_hex((char*)&character)) {
                    this->i2c_msg->tx_data[this->i2c_msg->num_tx_bytes] = this->i2c_msg->tx_data[this->i2c_msg->num_tx_bytes] | character;
                    this->buffer_state = GET_SPACE_BAR;
                    this->buffer_idx++;
                    this->i2c_msg->num_tx_bytes++;
                }

            } else {

                UARTprintf("\r\nError: too many bytes\r\n");
                this->buffer_state = START_SCRIPTER;
                this->buffer_idx = 0;

            }
        } else {
            this->send_bell();
        }

        break;
    case GET_SPACE_BAR :

        if (' ' == character) {

            this->buffer_state = GET_NIBBLE_0;
            UARTprintf(" ");

        } else if (']' == character) {

            UARTprintf("]\r\n");
            this->buffer_state = START_SCRIPTER;
            this->i2c_msg->num_tx_bytes = this->buffer_idx-1;
            this->buffer_idx = 0;
            xSemaphoreGive(this->send_semphr);

        } else {

            this->send_bell();

        }
        break;

    default :
        assert(0);
        break;
    }




} // End I2cScripterTask::draw_input

void I2cScripterTask::draw_reset(void) {

    this->buffer_idx = 0;
    this->buffer_state = START_SCRIPTER;
    TextCtl::cursor_pos(START_ROW, 0);
    TextCtl::clear_below_line();

    UARTprintf("Press r to reset\r\n");
    UARTprintf("Transmit messages in the format [xx xx xx ...] starting with address\r\n");



} // End I2cScripterTask::draw_help

void I2cScripterTask::draw_help(void) {

    UARTprintf("Description:\r\n");
    UARTprintf("    This page allows you only to transmit messages over I2C or SMBus. This page\r\n");
    UARTprintf("is useful for sending configuration messages to I2C and SMBus devices.\r\n\r\n");
    UARTprintf("Operation:\r\n");
    UARTprintf("  Transmit messages in the format [xx xx xx ...] starting with the xx 7 bit address.\r\n");
    UARTprintf("Troubleshooting:\r\n");
    UARTprintf("    Make sure that the SCL, SDA, GND and power lines are properly connected.\r\n");
    UARTprintf("Observe the bus voltage and make sure that the PS voltage you configured at the\r\n");
    UARTprintf("start up menu was configured properly. Ensure that you have pull-up resistors\r\n");
    UARTprintf("on the I2C bus, either from the configuration menu or external, and they are the\r\n");
    UARTprintf("proper value. If all else fails, revert to the I2C search page to make sure \r\n");
    UARTprintf("the device is detected on the bus.");


} // End I2cScripterTask::draw_help
