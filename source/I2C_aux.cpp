/*
 * I2C_aux.cpp
 *
 *  Created on: Jul 10, 2022
 *      Author: steph
 */
#include <stdint.h>
#include "sysctl.h"
#include "driverlib/i2c.h"
#include "gpio.h"
#include "driverlib/inc/hw_i2c.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/inc/hw_types.h"
#include "inc/hw_ints.h"
#include "I2C_aux.hpp"

static SemaphoreHandle_t nine_clk_semphr = NULL;
static const uint32_t BOTH_LINES_UP = 0x3;

I2cMsgAux::I2cMsgAux(void) {
    semphr = nullptr;
}

I2cAux::I2cAux(i2c_config_t* config) {

   this->logger = ErrorLogger::get_instance();

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C Aux",                            /* Text name for the task. */
                80,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                &this->task_handle );                      /* Used to pass out the created task's handle. */


    this->config = config;

    SysCtlPeripheralEnable(config->i2c_peripheral);
    SysCtlPeripheralEnable(config->gpio_peripheral);

    while(!SysCtlPeripheralReady(config->i2c_peripheral));
    while(!SysCtlPeripheralReady(config->gpio_peripheral));


    GPIOPinConfigure(config->i2c_scl_pin);
    GPIOPinConfigure(config->i2c_data_pin);

    GPIOPinTypeI2CSCL(config->gpio_base, config->gpio_scl_pin);
    GPIOPinTypeI2C(config->gpio_base, config->gpio_data_pin);

    I2CMasterInitExpClk(config->base, SysCtlClockGet(), false);

    I2CMasterTimeoutSet(config->base, 0x7d);

    this->i2c_msg_queue = xQueueCreate(20, sizeof(I2cMsgAux*));

    nine_clk_semphr = xSemaphoreCreateBinary();
    this->nine_clk_count = 0;

    this->addr_ack_err = logger->create_error("I2C3", "No ack from address");
    this->data_ack_err = logger->create_error("I2C3", "No ack from data");
    this->arb_lost_err = logger->create_error("I2C3", "Arbitration lost");
    this->clk_tout_err = logger->create_error("I2C3", "Clock timeout");
    this->clk_tout_err = logger->create_error("I2C3", "Line state low");

    this->bytes_rxed = 0;
    this->bytes_txed = 0;

} // End I2cAux::I2cAux

bool I2cAux::add_i2c_msg(I2cMsgAux* i2c_msg_ptr) {
    return xQueueSend(this->i2c_msg_queue, &i2c_msg_ptr, 0);
} // End I2cAux::add_i2c_msg

void I2cAux::taskfunwrapper(void* parm){
    (static_cast<I2cAux*>(parm))->task((I2cAux*)parm);
} // End I2cAux::taskfunwrapper

void I2cAux::task(I2cAux* this_ptr) {

    while(1) {

        switch(this_ptr->i2c_state) {
        case I2C_IDLE:

            xQueueReceive(this_ptr->i2c_msg_queue, &this_ptr->i2c_msg, portMAX_DELAY);

            assert(this_ptr->i2c_msg);

            if (BOTH_LINES_UP != I2CMasterLineStateGet(this_ptr->config->base)) {
                this_ptr->i2c_state = I2C_FINISH;
                this_ptr->i2c_msg->errors = PULL_UP_ERR;
                this_ptr->logger->set_error(this_ptr->pull_up_err);
                break;
            }

             this_ptr->i2c_msg->state = i2c_processing;
            this_ptr->i2c_msg->errors = NONE;
            this_ptr->bytes_rxed = 0;
            this_ptr->bytes_txed = 0;

            if(this_ptr->i2c_msg->num_tx_bytes > 0 ) {

                I2CMasterSlaveAddrSet(this_ptr->config->base,
                                      this_ptr->i2c_msg->address,
                                      false);

            } else if (this_ptr->i2c_msg->num_rx_bytes > 0) {

                I2CMasterSlaveAddrSet(this_ptr->config->base,
                                      this_ptr->i2c_msg->address,
                                      true);

            }

            if (0 == this_ptr->i2c_msg->num_tx_bytes) {
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_STOP);
                this_ptr->i2c_state = I2C_FINISH;
            } else if (1 == this_ptr->i2c_msg->num_tx_bytes) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[0]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_SINGLE_SEND);
                this_ptr->bytes_txed++;

                if (this_ptr->i2c_msg->num_rx_bytes > 0 ) {
                    this_ptr->i2c_state = I2C_RECEIVE_START;
                } else {
                    this_ptr->i2c_state = I2C_FINISH;
                }

            } else if (i2c_msg->num_tx_bytes > 1) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[0]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_START);
                this_ptr->bytes_txed++;
                this_ptr->i2c_state = I2C_SEND;

            } else if (this_ptr->i2c_msg->num_rx_bytes >= 1) {

                this_ptr->i2c_state = I2C_RECEIVE_START;

            }

            break;

        case I2C_SEND:

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if(log_errors(this_ptr)){
                this_ptr->i2c_state = I2C_NINE_CLOCK;
                break;
            }

            if (1 == this_ptr->i2c_msg->num_tx_bytes - this_ptr->bytes_txed) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[this_ptr->bytes_txed]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_FINISH);
                this_ptr->bytes_txed++;

                if (0 == this_ptr->i2c_msg->num_rx_bytes) {

                    this_ptr->i2c_state = I2C_FINISH;

                } else {

                    this_ptr->i2c_state = I2C_RECEIVE_START;

                }

            } else if (1 < this_ptr->i2c_msg->num_tx_bytes - this_ptr->bytes_txed) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[this_ptr->bytes_txed]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_CONT);
                this_ptr->bytes_txed++;
                this_ptr->i2c_state = I2C_SEND;

            }

            break;

        case I2C_RECEIVE_START :

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if(log_errors(this_ptr)){
                this_ptr->i2c_state = I2C_NINE_CLOCK;
                break;
            }

            I2CMasterSlaveAddrSet(this_ptr->config->base,
                                  this_ptr->i2c_msg->address,
                                  true);

            if (this_ptr->i2c_msg->num_rx_bytes > 1) {

                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_RECEIVE_START);
                this_ptr->i2c_state = I2C_RECEIVE;

            } else if (1 == this_ptr->i2c_msg->num_rx_bytes) {

                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_SINGLE_RECEIVE);
                this_ptr->i2c_state = I2C_RECEIVE;

            }

            break;

        case I2C_RECEIVE :

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if(log_errors(this_ptr)){
                this_ptr->i2c_state = I2C_NINE_CLOCK;
                break;
            }

            if (1 == this_ptr->i2c_msg->num_rx_bytes - this_ptr->bytes_rxed) {

                this_ptr->i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->bytes_rxed ] = I2CMasterDataGet(this_ptr->config->base);
                this_ptr->i2c_state = I2C_FINISH;

            } else if (2 == this_ptr->i2c_msg->num_rx_bytes - this_ptr->bytes_rxed) {

                i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->bytes_rxed] = I2CMasterDataGet(this_ptr->config->base);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
                this_ptr->i2c_state = I2C_RECEIVE;

            } else if (this_ptr->i2c_msg->num_rx_bytes - this_ptr->bytes_rxed > 2 ) {

                i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->bytes_rxed] = I2CMasterDataGet(this_ptr->config->base);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
                this_ptr->i2c_state = I2C_RECEIVE;

            }

            break;

        case I2C_FINISH :

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if (log_errors(this_ptr)) {
                this_ptr->i2c_state = I2C_NINE_CLOCK;
                break;
            }

            this_ptr->i2c_msg->state = i2c_finished;

            if (i2c_msg->semphr != nullptr) {
                xSemaphoreGive(i2c_msg->semphr);
            }

            this_ptr->i2c_state = I2C_IDLE;
            break;

        case I2C_NINE_CLOCK :

                if(this_ptr->nine_clk_count == 0) {
                    SysCtlPeripheralDisable(this_ptr->config->i2c_peripheral);

                    GPIODirModeSet(this_ptr->config->gpio_base,
                                       this_ptr->config->gpio_scl_pin,
                                       GPIO_DIR_MODE_OUT);

                    GPIOPadConfigSet(this_ptr->config->gpio_base,
                                         this_ptr->config->gpio_scl_pin,
                                         GPIO_STRENGTH_12MA,
                                         GPIO_PIN_TYPE_OD);

                    GPIOPinTypeGPIOOutputOD(this_ptr->config->gpio_base,
                                            this_ptr->config->gpio_scl_pin);
                }

                if (this_ptr->nine_clk_count < 9*2) {

                    GPIOPinWrite(config->gpio_base,
                                 config->gpio_scl_pin,
                                 (this_ptr->nine_clk_count % 2) == 0 ? 0 : config->gpio_scl_pin ) ;


                    this_ptr->nine_clk_count++;

                    xSemaphoreTake( nine_clk_semphr, portMAX_DELAY);

                } else {

                    SysCtlPeripheralEnable(this_ptr->config->i2c_peripheral);
                    while(!SysCtlPeripheralReady(this_ptr->config->i2c_peripheral));

                    GPIOPinConfigure(this_ptr->config->i2c_scl_pin);
                    GPIOPinConfigure(this_ptr->config->i2c_data_pin);

                    GPIOPinTypeI2CSCL(this_ptr->config->gpio_base, config->gpio_scl_pin);
                    GPIOPinTypeI2C(this_ptr->config->gpio_base, config->gpio_data_pin);

                    I2CMasterTimeoutSet(this_ptr->config->base, 0x7d);

                    this_ptr->nine_clk_count = 0;

                    this_ptr->i2c_state = I2C_IDLE;

                    if (i2c_msg->semphr != nullptr) {
                        xSemaphoreGive(i2c_msg->semphr);
                    }

                }

            break;

        default :

            assert(0);
            break;
        }

        vTaskDelay(0);

    }

} // End i2c_task

bool I2cAux::log_errors(I2cAux* this_ptr) {

    uint32_t status = I2CMasterErr(I2C3_BASE);

    // THE ORDER OF THESE IF STATEMENTS MATTER!!!


    if( I2C_MASTER_ERR_DATA_ACK  == (status &  I2C_MASTER_ERR_DATA_ACK )) {

        this_ptr->logger->set_error(data_ack_err);
        this_ptr->i2c_msg->errors = DATA_NACK_ERR;
    }


    if(I2C_MASTER_ERR_CLK_TOUT == (status & I2C_MASTER_ERR_CLK_TOUT)) {
        this_ptr->logger->set_error(clk_tout_err);
        this_ptr->i2c_msg->errors = TIMEOUT_ERR;
    }


    if(I2C_MASTER_ERR_ADDR_ACK == (status & I2C_MASTER_ERR_ADDR_ACK)) {

        this_ptr->logger->set_error(addr_ack_err);
        this_ptr->i2c_msg->errors = ADDR_NACK_ERR;
    }


    if(I2C_MASTER_ERR_ARB_LOST == (status & I2C_MASTER_ERR_ARB_LOST)) {
        this_ptr->logger->set_error(arb_lost_err);
        this_ptr->i2c_msg->errors = ARB_LOST_ERR;
    }



    if (status) {
        return true;
    } else {
        return false;
    }

} // End log_errors


