/*
 * I2C_task.cpp
 *
 *  Created on: Jul 27, 2021
 *      Author: steph
 */

#include <string>

#include "I2C_task.hpp"

#include "sysctl.h"
#include "driverlib/i2c.h"
#include "gpio.h"
#include "driverlib/inc/hw_i2c.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/timer.h"

#include "uartstdio.h"


static const uint32_t NUM_OF_TX_BYTES = 10;
static const uint32_t NUM_OF_RX_BYTES = 10;

static const uint32_t BOTH_LINES_UP = 0x3;

static volatile bool error_flag = false;
static volatile i2c_errors_e i2c_error_status = NONE;

void set_i2c_clock_speed(uint32_t index) {

    if (0 == index) {

        I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

    } else if (1 == index) {

        I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), true);

    } else if (2 == index){

        I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), true);

        HWREG(I2C1_BASE + I2C_O_MTPR) = ((SysCtlClockGet() + (2 * 10 * 1000000) - 1) /
                                          (2 * 10 * 1000000)) - 1;

    } else  {
        assert(0);
    }

} // End I2cTask::set_clock_speed

I2cMsg::I2cMsg(i2c_msg_type_t type) {

    this->address = 0;
    this->tx_data = NULL;
    this->num_tx_bytes = 0;
    this->bytes_txed = 0;
    this->rx_data = NULL;
    this->num_rx_bytes = 0;
    this->bytes_rxed = 0;
    this->state = i2c_ready;
    this->tx_time = 0;
    this->type = type;
    this->monitored = false;
    this->errors = NONE;
    this->active = false;

} // End I2cMsg::I2cMsg

I2cTask::I2cTask(i2c_config_t* config) : ConsolePage("I2C Command",
                                                     portMAX_DELAY,
                                                     false) {

   this->logger = ErrorLogger::get_instance();

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C Task",                            /* Text name for the task. */
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

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));

    TimerClockSourceSet(TIMER1_BASE, TIMER_CLOCK_SYSTEM);

    TimerDisable(TIMER1_BASE, TIMER_BOTH);

    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    TimerEnable(TIMER1_BASE, TIMER_BOTH);

    this->i2c_msg_queue = xQueueCreate(20, sizeof(I2cMsg*));

    this->i2c_cmd_state = GET_MONITOR_STATUS;
    this->byte_buffer_index = 0;
    this->i2c_cmd_msg = new I2cMsg(command_msg);

    for (uint32_t index=0; index<NUM_OF_MONITORED_MSGS; index++) {
        this->i2c_monitor_msgs.push_back(new I2cMsg(normal_msg));
        this->i2c_monitor_msgs[index]->tx_data = new uint8_t[NUM_OF_TX_BYTES];
        this->i2c_monitor_msgs[index]->rx_data = new uint8_t[NUM_OF_RX_BYTES];
    }

    this->i2c_cmd_msg->rx_data = new uint8_t[NUM_OF_RX_BYTES];
    this->i2c_cmd_msg->tx_data = new uint8_t[NUM_OF_TX_BYTES];

    this->byte_counter = 0;
    this->byte_buffer_index = 0;
    this->i2c_monitor_index = 0;

    this->addr_ack_err = logger->create_error("I2C0", "No ack from address");
    this->data_ack_err = logger->create_error("I2C0", "No ack from data");
    this->arb_lost_err = logger->create_error("I2C0", "Arbitration lost");
    this->clk_tout_err = logger->create_error("I2C0", "Clock timeout");
    this->clk_tout_err = logger->create_error("I2C0", "Line state low");

} // End I2cTask::I2cTask

bool I2cTask::add_i2c_msg(I2cMsg* i2c_msg_ptr) {

    return xQueueSend(this->i2c_msg_queue, &i2c_msg_ptr, 0);

} // End I2cTask::add_i2c_msg

std::vector<I2cMsg*>* I2cTask::get_vector(void) {
    return &this->i2c_monitor_msgs;
} // End get_vector

void I2cTask::taskfunwrapper(void* parm){
    (static_cast<I2cTask*>(parm))->task((I2cTask*)parm);
} // End I2cTask::taskfunwrapper

void I2cTask::task(I2cTask* this_ptr) {

    uint32_t message_time = 0;

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
            this_ptr->i2c_msg->bytes_rxed = 0;
            this_ptr->i2c_msg->bytes_txed = 0;

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
                this_ptr->i2c_msg->bytes_txed++;

                if (this_ptr->i2c_msg->num_rx_bytes > 0 ) {
                    this_ptr->i2c_state = I2C_RECEIVE_START;
                } else {
                    this_ptr->i2c_state = I2C_FINISH;
                }

            } else if (i2c_msg->num_tx_bytes > 1) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[0]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_START);
                this_ptr->i2c_msg->bytes_txed++;
                this_ptr->i2c_state = I2C_SEND;

            } else if (this_ptr->i2c_msg->num_rx_bytes >= 1) {

                this_ptr->i2c_state = I2C_RECEIVE_START;

            }

            TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet());

            break;

        case I2C_SEND:

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if(log_errors(this_ptr)){
                this_ptr->i2c_state = I2C_FINISH;
                break;
            }

            if (1 == this_ptr->i2c_msg->num_tx_bytes - this_ptr->i2c_msg->bytes_txed) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[i2c_msg->bytes_txed]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_FINISH);
                this_ptr->i2c_msg->bytes_txed++;

                if (0 == this_ptr->i2c_msg->num_rx_bytes) {

                    this_ptr->i2c_state = I2C_FINISH;

                } else {

                    this_ptr->i2c_state = I2C_RECEIVE_START;

                }

            } else if (1 < this_ptr->i2c_msg->num_tx_bytes - this_ptr->i2c_msg->bytes_txed) {

                I2CMasterDataPut(this_ptr->config->base, this_ptr->i2c_msg->tx_data[i2c_msg->bytes_txed]);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_SEND_CONT);
                this_ptr->i2c_msg->bytes_txed++;
                this_ptr->i2c_state = I2C_SEND;

            }

            break;

        case I2C_RECEIVE_START :

            if (I2CMasterBusy(this_ptr->config->base)) break;

            if(log_errors(this_ptr)){
                this_ptr->i2c_state = I2C_FINISH;
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
                this_ptr->i2c_state = I2C_FINISH;
                break;
            }

            if (1 == this_ptr->i2c_msg->num_rx_bytes - this_ptr->i2c_msg->bytes_rxed) {

                this_ptr->i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->i2c_msg->bytes_rxed ] = I2CMasterDataGet(this_ptr->config->base);
                this_ptr->i2c_state = I2C_FINISH;

            } else if (2 == this_ptr->i2c_msg->num_rx_bytes - this_ptr->i2c_msg->bytes_rxed) {

                i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->i2c_msg->bytes_rxed] = I2CMasterDataGet(this_ptr->config->base);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
                this_ptr->i2c_state = I2C_RECEIVE;

            } else if (this_ptr->i2c_msg->num_rx_bytes - this_ptr->i2c_msg->bytes_rxed > 2 ) {

                i2c_msg->rx_data[this_ptr->i2c_msg->num_rx_bytes - ++this_ptr->i2c_msg->bytes_rxed] = I2CMasterDataGet(this_ptr->config->base);
                I2CMasterControl(this_ptr->config->base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
                this_ptr->i2c_state = I2C_RECEIVE;

            }

            break;

        case I2C_FINISH :

            if (I2CMasterBusy(this_ptr->config->base)) break;

            message_time = SysCtlClockGet() - TimerValueGet(TIMER1_BASE, TIMER_A);

            log_errors(this_ptr);

            if ((this_ptr->on_screen) && (false == this_ptr->i2c_msg->monitored)) {

                this_ptr->print_errors(this_ptr);

                UARTprintf("TX: ");
                for (uint32_t index=0; index<this_ptr->i2c_msg->bytes_txed; index++) {
                    UARTprintf("0x%2x ", this_ptr->i2c_msg->tx_data[index]);
                }

                UARTprintf("\r\nRX: ");
                for (uint32_t index=0; index<this_ptr->i2c_msg->bytes_rxed; index++) {
                    UARTprintf("0x%2x ", this_ptr->i2c_msg->rx_data[index]);
                }

                UARTprintf("\r\nMeasured transaction time: %d microseconds\r\n", message_time/16);

                UARTprintf("\r\nMonitor register? y/n : ");
            }

            this_ptr->i2c_msg->state = i2c_finished;

            this_ptr->i2c_state = I2C_IDLE;

            break;

        default :

            assert(0);
            break;
        }

        vTaskDelay(0);

    }

} // End i2c_task

void I2cTask::print_errors(I2cTask* this_ptr) {


    switch(this_ptr->i2c_msg->errors) {
    case NONE :
        UARTprintf("\r\nMessage transmitted with no errors\r\n");
        break;
    case ADDR_NACK_ERR :
        UARTprintf("\r\nError: Address Nacked\r\n");
        break;
    case DATA_NACK_ERR :
        if (this_ptr->i2c_msg->num_tx_bytes != this_ptr->i2c_msg->bytes_txed) {
            UARTprintf("\r\nError: NACK on TX byte %d\r\n", this_ptr->i2c_msg->bytes_txed);
        } else if (this_ptr->i2c_msg->num_rx_bytes != this_ptr->i2c_msg->bytes_rxed) {
            UARTprintf("\r\nError: NACK on RX byte %d\r\n", this_ptr->i2c_msg->bytes_rxed);
        } else {
            assert(0);
        }
        break;
    case ARB_LOST_ERR :
        UARTprintf("\r\nError: Arbitration Lost\r\n");
        break;
    case TIMEOUT_ERR :
        if (this_ptr->i2c_msg->num_tx_bytes != this_ptr->i2c_msg->bytes_txed) {
            UARTprintf("\r\nError: Timeout on TX byte %d\r\n", this_ptr->i2c_msg->bytes_txed);
        } else if (this_ptr->i2c_msg->num_rx_bytes != this_ptr->i2c_msg->bytes_rxed) {
            UARTprintf("\r\nError: timeout on RX byte %d\r\n", this_ptr->i2c_msg->bytes_rxed);
        } else {
            assert(0);
        }
        break;
    case PULL_UP_ERR :
        UARTprintf("\r\nError: Lines are low, check pull ups\r\n");
        break;
    default :
        assert(0);
        break;
    }
}

bool I2cTask::log_errors(I2cTask* this_ptr) {

    uint32_t status = I2CMasterErr(I2C1_BASE);

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

static uint32_t ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
}

void I2cTask::draw_page(void) {
    UARTprintf("Monitor register? y/n :");
}
void I2cTask::draw_data(void) {

}
void I2cTask::draw_input(int character) {

    switch(i2c_cmd_state) {
    case GET_MONITOR_STATUS :
        if (('y' == character) && (this->i2c_monitor_index < NUM_OF_MONITORED_MSGS)) {

            this->i2c_cmd_state = GET_ADDRESS;
            this->i2c_monitor_msgs[this->i2c_monitor_index]->type = normal_msg;
            this->i2c_monitor_msgs[this->i2c_monitor_index]->monitored = true;
            UARTprintf("%c\n", character);
            UARTprintf("Enter Address : 0x");
            this->monitored = true;

        } else if ('n' == character) {
            this->i2c_cmd_msg->type = command_msg;
            this->i2c_cmd_state = GET_ADDRESS;
            UARTprintf("%c\n", character);
            UARTprintf("Enter Address : 0x");
            this->monitored = false;

        } else if ('y'== character && this->i2c_monitor_index >= NUM_OF_MONITORED_MSGS) {
            UARTprintf("%c\r\n", character);
            UARTprintf("Error: too many monitored messages\r\n");
            UARTprintf("Monitor register? y/n : ");
        }

        break;

    case GET_ADDRESS :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (0 == this->byte_buffer_index) {

                this->byte_buffer = ascii_to_hex(character) << 4;
                UARTprintf("%c", character);
                this->byte_buffer_index++;
            } else {
                this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                this->byte_buffer_index = 0;
                UARTprintf("%c", character);
                UARTprintf("\n Enter number of tx bytes: ");
                if (this->monitored) {

                    this->i2c_monitor_msgs[this->i2c_monitor_index]->address = this->byte_buffer;

                } else {
                    this->i2c_cmd_msg->address = this->byte_buffer;
                }
                this->i2c_cmd_state = GET_NUM_TX_BYTES;
                this->byte_buffer_index = 0;
            }
        }
        break;
    case GET_NUM_TX_BYTES :

        if ((character >= '0' && character <= '9')){

            if (this->monitored) {
                this->i2c_monitor_msgs[this->i2c_monitor_index]->num_tx_bytes = character - '0';
            } else {
                this->i2c_cmd_msg->num_tx_bytes = character -'0';
            }
            UARTprintf("%c", character);
            UARTprintf("\nbyte 1 : 0x");
            this->i2c_cmd_state = GET_TX_BYTES;
        }
        break;
    case GET_TX_BYTES :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (this->monitored) {

                if(this->byte_counter < this->i2c_monitor_msgs[this->i2c_monitor_index]->num_tx_bytes) {

                    if (0 == this->byte_buffer_index) {

                        this->byte_buffer = ascii_to_hex(character) << 4;
                        this->byte_buffer_index++;
                        UARTprintf("%c", character);

                    } else {
                        this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                        this->i2c_monitor_msgs[this->i2c_monitor_index]->tx_data[this->byte_counter] = this->byte_buffer;
                        this->byte_buffer_index = 0;
                        this->byte_counter++;
                        UARTprintf("%c", character);
                        if(this->byte_counter < this->i2c_monitor_msgs[this->i2c_monitor_index]->num_tx_bytes) {
                            UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                        }
                    }
                }
            } else {

                if(this->byte_counter < this->i2c_cmd_msg->num_tx_bytes) {
                    if (0 == this->byte_buffer_index) {
                        this->byte_buffer = ascii_to_hex(character) << 4;
                        this->byte_buffer_index++;
                        UARTprintf("%c", character);

                    } else {
                        this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                        this->byte_buffer_index = 0;
                        this->i2c_cmd_msg->tx_data[this->byte_counter] = this->byte_buffer;

                        this->byte_counter++;
                        UARTprintf("%c", character);
                        if(this->byte_counter < this->i2c_cmd_msg->num_tx_bytes) {
                            UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                        }
                    }
                }
            }

            if (this->monitored) {
                if (this->byte_counter >= this->i2c_monitor_msgs[this->i2c_monitor_index]->num_tx_bytes ) {
                    this->byte_counter = 0;
                    this->i2c_cmd_state = GET_NUM_RX_BYTES;
                    this->byte_buffer_index = 0;
                    UARTprintf("\nEnter number of rx bytes: ");
                }

            } else {
                if (this->byte_counter >= this->i2c_cmd_msg->num_tx_bytes ) {
                    this->byte_counter = 0;
                    this->i2c_cmd_state = GET_NUM_RX_BYTES;
                    this->byte_buffer_index = 0;
                    UARTprintf("\nEnter number of rx bytes: ");
                }
            }
        }

        break;

    case GET_NUM_RX_BYTES :

        if ((character >= '0' && character <= '9')){

            if (this->monitored) {
                this->i2c_monitor_msgs[this->i2c_monitor_index]->num_rx_bytes = character - '0';
            } else {
                this->i2c_cmd_msg->num_rx_bytes = character - '0';
            }
            UARTprintf("%c", character);
            UARTprintf("\nPress Spacebar to send:\n\r");
            this->i2c_cmd_state = SEND_I2C_MSG;
        }

        break;

    case SEND_I2C_MSG:
        if (this->monitored) {
            if (' ' == character) {
                UARTprintf("\nTab to next page to see monitored register\n");
                UARTprintf("\r\nMonitor register? y/n : ");
                this->i2c_monitor_msgs[this->i2c_monitor_index]->active = true;
                this->i2c_cmd_state = GET_MONITOR_STATUS;
                this->add_i2c_msg(this->i2c_monitor_msgs[this->i2c_monitor_index]);
                this->i2c_monitor_index++;
                this->monitored = false;


            }
        } else {
            if (' ' == character) {
                this->add_i2c_msg(this->i2c_cmd_msg);
                this->i2c_cmd_state = GET_MONITOR_STATUS;
                this->monitored = false;

            }
        }
        break;

    default :
        assert(0);
        break;
    }
} // End I2cTask::draw_input

void I2cTask::reset_monitor_index(void) {

    this->i2c_monitor_index = 0;
    this->i2c_monitor_msgs.clear();

} // End I2cTask::reset_monitor_index

void I2cTask::draw_help(void) {

}

void I2cTask::draw_reset(void) {
    this->i2c_cmd_state = GET_MONITOR_STATUS;
    this->byte_counter = 0;
    this->byte_buffer_index = 0;
    this->byte_buffer = 0;
    UARTprintf("\n\rMonitor register? y/n :");
}
