/*
 * I2C_sniffer.cpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */


#include "I2C_sniffer.hpp"
#include "text_controls.hpp"
#include "uartstdio.h"
#include <assert.h>
#include <console_uart.hpp>

static uint8_t I2C_TEST_REG = 0x00;

void I2cSnifferTask::taskfunwrapper(void* parm){
    (static_cast<I2cSnifferTask*>(parm))->task((I2cSnifferTask*)parm);
} // End I2cSnifferTask::taskfunwrapper


I2cSnifferTask::I2cSnifferTask(I2cTask* i2c) : ConsolePage("I2C Sniffer",
                                                           portMAX_DELAY,
                                                           true) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C Sniff",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->i2c = i2c;

    this-> start_semphr = xSemaphoreCreateBinary();

    this->i2c_msg = new I2cMsg(normal_msg);

} // End I2cSnifferTask::I2cSnifferTask


void I2cSnifferTask::task(I2cSnifferTask* this_ptr) {

    while(1){

        xSemaphoreTake(this->start_semphr, portMAX_DELAY);

        for (uint32_t index=0; index < 0x100; index++) {

            //Set i2c_msg
            this->i2c_msg->address = index;
            this->i2c_msg->num_tx_bytes = 1;
            this->i2c_msg->num_rx_bytes = 0;
            this->i2c_msg->tx_data = &I2C_TEST_REG;
            this->i2c_msg->state = i2c_ready;

            this->i2c->add_i2c_msg(this->i2c_msg);

            while (this->i2c_msg->state != i2c_finished) {
                vTaskDelay(1);
            }

            if (this->on_screen) {

                if (this->i2c_msg->errors == PULL_UP_ERR) {

                    UARTprintf("ERROR: I2c lines low, check pull ups\r\n");
                    break;

                } else if (this->i2c_msg->errors == ADDR_NACK_ERR) {

                    TextCtl::text_color(TextCtl::red_text);
                    UARTprintf("0x%x, ", index);
                    TextCtl::text_color(TextCtl::white_text);

                } else if (this->i2c_msg->errors == NONE) {

                    TextCtl::text_color(TextCtl::green_text);
                    UARTprintf("0x%x, ", index);
                    TextCtl::text_color(TextCtl::white_text);

                } else if (this->i2c_msg->errors == TIMEOUT_ERR){

                    UARTprintf("TIMEOUT @ 0x%x, ", index);

                } else if (this->i2c_msg->errors == ARB_LOST_ERR) {

                    UARTprintf("ARB LOST @ 0x%x, ", index);
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
    }
}
