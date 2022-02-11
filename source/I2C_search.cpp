/*
 * I2C_sniffer.cpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */


#include "text_controls.hpp"
#include "uartstdio.h"
#include <assert.h>
#include <console_uart.hpp>
#include <I2C_search.hpp>

static uint8_t I2C_TEST_REG = 0x00;

void I2cSnifferTask::taskfunwrapper(void* parm){
    (static_cast<I2cSnifferTask*>(parm))->task((I2cSnifferTask*)parm);
} // End I2cSnifferTask::taskfunwrapper


I2cSnifferTask::I2cSnifferTask(I2cTask* i2c) : ConsolePage("I2C Search",
                                                           portMAX_DELAY,
                                                           false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C Search",                                     /* Text name for the task. */
                80,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->i2c = i2c;

    this-> start_semphr = xSemaphoreCreateBinary();

    this->i2c_msg = new I2cMsg(search_msg);

} // End I2cSnifferTask::I2cSnifferTask


void I2cSnifferTask::task(I2cSnifferTask* this_ptr) {

    while(1){

        xSemaphoreTake(this->start_semphr, portMAX_DELAY);

        for (uint32_t index=0; index <= 0x7f; index++) {

            //Set i2c_msg
            this->i2c_msg->address = index;
            this->i2c_msg->num_tx_bytes = 1;
            this->i2c_msg->num_rx_bytes = 0;
            this->i2c_msg->tx_data = &I2C_TEST_REG;
            this->i2c_msg->state = i2c_ready;

            this->i2c->add_i2c_msg(this->i2c_msg);

            while (this->i2c_msg->state != i2c_finished) {
                vTaskDelay(5);
            }

            if (this->on_screen) {

                if (this->i2c_msg->errors == PULL_UP_ERR) {

                    UARTprintf("ERROR: I2c lines low, check pull ups\r\n");
                    break;

                } else if (this->i2c_msg->errors == ADDR_NACK_ERR) {

                    TextCtl::text_color(TextCtl::red_text);
                    UARTprintf("0x%2x, ", index);
                    TextCtl::text_color(TextCtl::white_text);

                } else if (this->i2c_msg->errors == NONE) {

                    TextCtl::text_color(TextCtl::green_text);
                    UARTprintf("0x%2x, ", index);
                    TextCtl::text_color(TextCtl::white_text);

                } else if (this->i2c_msg->errors == TIMEOUT_ERR){

                    UARTprintf("TIMEOUT @ 0x%2x, ", index);

                } else if (this->i2c_msg->errors == ARB_LOST_ERR) {

                    UARTprintf("ARB LOST @ 0x%2x, ", index);
                    break;
                }
            }
        }

        UARTprintf("\r\n\n Press spacebar to sniff I2C addresses\r\n\n");
    }

} // End AdcTask::task

void I2cSnifferTask::draw_page(void) {

    UARTprintf("Press spacebar to sniff I2C addresses\r\n\n");

}
void I2cSnifferTask::draw_data(void) {

}
void I2cSnifferTask::draw_input(int character) {

    if (' ' == character) {
        xSemaphoreGive(this->start_semphr);
    } else {
        this->send_bell();
    }
}

void I2cSnifferTask::draw_help(void) {

    UARTprintf("Description:\r\n");
    UARTprintf("    This feature allows you to search all of the 7 bit addresses\r\n");
    UARTprintf("on a SMBUS or I2C bus.\r\n\r\n");
    UARTprintf("Operation:\r\n");
    UARTprintf("    Press space-bar. The red addresses are not detected on the bus and\r\n");
    UARTprintf("and the green addresses are detected\r\n\r\n");
    UARTprintf("Troubleshooting:\r\n");
    UARTprintf("    Make sure that the SCL, SDA, GND and power lines are properly connected.\r\n");
    UARTprintf("Observe the bus voltage and make sure that the PS voltage you configured at the\r\n");
    UARTprintf("start up menu was configured properly. Ensure that you have pull-up resistors\r\n");
    UARTprintf("on the I2C bus and they are the proper value.\r\n");

}

void I2cSnifferTask::draw_reset(void) {

}
