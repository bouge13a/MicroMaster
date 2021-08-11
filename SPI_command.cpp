/*
 * SPI_command.cpp
 *
 *  Created on: Aug 9, 2021
 *      Author: steph
 */


#include "SPI_command.hpp"
#include "uartstdio.h"

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

static const uint32_t SPI_TX_Q_NUM = 20;
static const uint32_t SPI_RX_Q_NUM = 20;

static const uint32_t NUM_OF_TX_BYTES = 10;
static const uint32_t NUM_OF_RX_BYTES = 10;

static const uint32_t NUM_OF_MONITORED_MSGS = 5;

static volatile uint32_t spi0_status = 0;

static void spi0_int_handler(void) {

    spi0_status = SSIIntStatus(SSI0_BASE, false);
    SSIIntClear(SSI0_BASE, SSI_RXTO | SSI_RXOR);

}

SpiMsg::SpiMsg(spi_msg_type_e msg_type) {

    this->msg_type = msg_type;
}

void SpiCmdTask::taskfunwrapper(void* parm){
    (static_cast<SpiCmdTask*>(parm))->task((SpiCmdTask*)parm);
} // End SpiCmdTask::taskfunwrapper

SpiCmdTask::SpiCmdTask(void) : ConsolePage("SPI Command",
                                           portMAX_DELAY,
                                           false)  {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Test",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

    SSIEnable(SSI0_BASE);

    SSIIntRegister(SSI0_BASE, spi0_int_handler);
    SSIIntEnable(SSI0_BASE, SSI_RXTO | SSI_RXOR);

    this->spi_tx_queue = xQueueCreate(20, sizeof(SpiMsg*));

//    this->spi_msg = new SpiMsg(spi_command_msg);
//    this->spi_msg->tx_bytes = new uint32_t[SPI_TX_Q_NUM];
//    this->spi_msg->rx_bytes = new uint32_t[SPI_RX_Q_NUM];
//    this->spi_msg->bytes_txed = 0;
//    this->spi_msg->bytes_rxed = 0;
//    this->spi_msg->state = spi_ready;
//    this->spi_msg->active = false;
//    this->spi_msg->monitored = false;
//    this->spi_msg->errors = SPI_NO_ERRORS;

    this->byte_buffer = 0;
    this->byte_buffer_index = 0;
    this->byte_counter = 0;
    this->spi_monitor_index = 0;
    this->monitored = false;

    this->spi_cmd_msg = new SpiMsg(spi_command_msg);
    this->spi_cmd_msg->tx_bytes = new uint32_t[SPI_TX_Q_NUM];
    this->spi_cmd_msg->rx_bytes = new uint32_t[SPI_RX_Q_NUM];
    this->spi_cmd_msg->bytes_txed = 0;
    this->spi_cmd_msg->bytes_rxed = 0;
    this->spi_cmd_msg->state = spi_ready;
    this->spi_cmd_msg->active = false;
    this->spi_cmd_msg->monitored = false;
    this->spi_cmd_msg->errors = SPI_NO_ERRORS;


    for (uint32_t index=0; index<NUM_OF_MONITORED_MSGS; index++) {
        this->spi_monitor_msgs.push_back(new SpiMsg(spi_normal_msg));
        this->spi_monitor_msgs[index]->tx_bytes = new uint32_t[NUM_OF_TX_BYTES];
        this->spi_monitor_msgs[index]->rx_bytes = new uint32_t[NUM_OF_RX_BYTES];
    }

    while(SSIDataGetNonBlocking(SSI0_BASE, &this->spi_msg->rx_bytes[0]));

    this->logger = ErrorLogger::get_instance();

    this->rx_timeout_err = this->logger->create_error("SPI", "RX timeout");
    this->rx_overrun_err = this->logger->create_error("SPI", "RX overrun");

    this->cmd_state = SPI_GET_MONITOR_STATUS;

} // End SpiCmdTask(void)

bool SpiCmdTask::add_spi_msg(SpiMsg* spi_msg) {

    return xQueueSend(this->spi_tx_queue, &spi_msg, 0);

}

void SpiCmdTask::task(SpiCmdTask* this_ptr) {

    while(1) {

        switch (this_ptr->state) {
        case SPI_IDLE :

            xQueueReceive(spi_tx_queue, &this_ptr->spi_msg, portMAX_DELAY);

            assert(this_ptr->spi_msg);

            this_ptr->spi_msg->bytes_txed = 0;
            this_ptr->spi_msg->bytes_rxed = 0;

            this_ptr->state = SPI_SEND;

            this_ptr->spi_msg->state = spi_processing;

            break;

        case SPI_SEND :

            while(SSIBusy(SSI0_BASE)) {
                break;
            }

            if(log_errors(this_ptr)){
                this_ptr->state = SPI_FINISH;
                break;
            }

            if (this_ptr->spi_msg->bytes_txed < this_ptr->spi_msg->num_tx_bytes) {

                SSIDataPutNonBlocking(SSI0_BASE,
                                      this->spi_msg->tx_bytes[this_ptr->spi_msg->bytes_txed]);

                this_ptr->spi_msg->bytes_txed++;

            } else if (this_ptr->spi_msg->num_rx_bytes > 0) {

                this_ptr->state = SPI_RECEIVE;

            } else {

                this_ptr->state = SPI_FINISH;
            }

            break;

        case SPI_RECEIVE :

            while(SSIBusy(SSI0_BASE)) {
                break;
            }

            if(log_errors(this_ptr)){
                this_ptr->state = SPI_FINISH;
                break;
            }

            if (this_ptr->spi_msg->bytes_rxed < this_ptr->spi_msg->num_rx_bytes) {

                SSIDataGetNonBlocking(SSI0_BASE,
                                      &this->spi_msg->rx_bytes[this_ptr->spi_msg->bytes_rxed]);

                this_ptr->spi_msg->bytes_rxed++;

            } else {

                this_ptr->state = SPI_FINISH;

            }

            break;

        case SPI_FINISH :

            while(SSIBusy(SSI0_BASE)) {
                break;
            }

            log_errors(this_ptr);

            this_ptr->spi_msg->state = spi_finished;
            this_ptr->state = SPI_IDLE;

            if (this_ptr->on_screen == true && this_ptr->spi_msg->monitored == false ) {

                this_ptr->print_errors(this_ptr);

                UARTprintf("TX: ");
                for (uint32_t index=0; index<this_ptr->spi_msg->bytes_txed; index++) {
                    UARTprintf("0x%x ", this_ptr->spi_msg->tx_bytes[index]);
                }

                UARTprintf("RX: ");
                for (uint32_t index=0; index<this_ptr->spi_msg->bytes_rxed; index++) {
                    UARTprintf("0x%x ", this_ptr->spi_msg->rx_bytes[index]);
                }

                UARTprintf("\r\n\nMonitor message? y/n : ");

            }


            break;

        default :
            break;
        }
    }
}

void SpiCmdTask::print_errors(SpiCmdTask* this_ptr) {

    switch(this_ptr->spi_msg->errors){
    case SPI_NO_ERRORS :
        UARTprintf("\r\nMessage transmitted with no errors\r\n");
        break;
    case SPI_TIMEOUT_ERR :
        this_ptr->logger->set_error(this_ptr->rx_timeout_err);
        UARTprintf("\r\nMessage RX timed out");
        break;
    case SPI_OVERRUN_ERR :
        this_ptr->logger->set_error(this_ptr->rx_overrun_err);
        UARTprintf("\r\nMessage RX overran");
        break;
    default :
        assert(0);
        break;
    }

}


bool SpiCmdTask::log_errors(SpiCmdTask* this_ptr) {

    if (SPI_NO_ERRORS == SPI_NO_ERRORS & spi0_status) {

        this_ptr->spi_msg->errors = SPI_NO_ERRORS;
        return false;

    } else if (SPI_OVERRUN_ERR == SPI_OVERRUN_ERR & spi0_status) {

        this_ptr->spi_msg->errors = SPI_OVERRUN_ERR;
        this->logger->set_error(rx_overrun_err);
        spi0_status = SPI_NO_ERRORS;
        return true;

    } else if (SPI_TIMEOUT_ERR == SPI_TIMEOUT_ERR & spi0_status) {

        this_ptr->spi_msg->errors = SPI_TIMEOUT_ERR;
        this->logger->set_error(rx_timeout_err);
        spi0_status = SPI_NO_ERRORS;
        return true;

    } else {

        return false;
    }
}

uint32_t SpiCmdTask::ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
}

void SpiCmdTask::draw_page(void) {

    UARTprintf("Monitor message? y/n : ");

}

void SpiCmdTask::draw_data(void) {

}

void SpiCmdTask::draw_input(int character) {

    switch(this->cmd_state) {
    case SPI_GET_MONITOR_STATUS :

        if (('y' == character) && (this->spi_monitor_index < NUM_OF_MONITORED_MSGS)) {

            this->cmd_state = SPI_GET_NUM_TX_BYTES;
            this->spi_monitor_msgs[this->spi_monitor_index]->monitored = true;
            UARTprintf("%c\n", character);
            UARTprintf("Enter number of TX bytes : ");
            this->monitored = true;

        } else if ('n' == character){

            this->cmd_state = SPI_GET_NUM_TX_BYTES;
            this->spi_cmd_msg->monitored = false;
            UARTprintf("%c\n", character);
            UARTprintf("Enter number of TX bytes : ");
            this->monitored = false;

        }

        break;
    case SPI_GET_NUM_TX_BYTES :

        if ((character >= '0' && character <= '9')){

            if (this->monitored) {
                this->spi_monitor_msgs[this->spi_monitor_index]->num_tx_bytes = character - '0';
            } else {
                this->spi_cmd_msg->num_tx_bytes = character - '0';
            }

            UARTprintf("%c", character);
            UARTprintf("\nbyte 1 : 0x");
            this->cmd_state = SPI_TX_BYTES;

        }
        break;
    case SPI_TX_BYTES :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (this->monitored) {
                if(this->byte_counter < this->spi_monitor_msgs[this->spi_monitor_index]->num_tx_bytes) {

                    if (0 == this->byte_buffer_index) {

                        this->spi_msg->tx_bytes[0] = 0;
                        this->byte_buffer = ascii_to_hex(character) << 4;
                        this->byte_buffer_index++;
                        UARTprintf("%c", character);

                    } else {

                        this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                        this->spi_monitor_msgs[this->spi_monitor_index]->tx_bytes[this->byte_counter] = this->byte_buffer;
                        this->byte_buffer_index = 0;
                        this->byte_counter++;
                        UARTprintf("%c", character);
                        if(this->byte_counter < this->spi_monitor_msgs[this->spi_monitor_index]->num_tx_bytes) {
                            UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                        }
                    }
                }
            } else {

                if(this->byte_counter < this->spi_cmd_msg->num_tx_bytes) {

                    if (0 == this->byte_buffer_index) {
                        this->spi_msg->tx_bytes[0] = 0;
                        this->byte_buffer = ascii_to_hex(character) << 4;
                        this->byte_buffer_index++;
                        UARTprintf("%c", character);

                    } else {
                        this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                        this->byte_buffer_index = 0;
                        this->spi_cmd_msg->tx_bytes[this->byte_counter] = this->byte_buffer;

                        this->byte_counter++;
                        UARTprintf("%c", character);
                        if(this->byte_counter < this->spi_cmd_msg->num_tx_bytes) {
                            UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                        }
                    }
                }
            }


            if (this->monitored) {
                if (this->byte_counter >= this->spi_monitor_msgs[this->spi_monitor_index]->num_tx_bytes ) {
                    this->byte_counter = 0;
                    this->cmd_state = SPI_GET_NUM_RX_BYTES;
                    this->byte_buffer_index = 0;
                    UARTprintf("\nEnter number of RX bytes: ");
                }

            } else {

                if (this->byte_counter >= this->spi_cmd_msg->num_tx_bytes ) {
                    this->byte_counter = 0;
                    this->cmd_state = SPI_GET_NUM_RX_BYTES;
                    this->byte_buffer_index = 0;
                    UARTprintf("\nEnter number of RX bytes: ");
                }
            }
        }

        break;
    case SPI_GET_NUM_RX_BYTES :

        if ((character >= '0' && character <= '9')){

            if (this->monitored) {
                this->spi_monitor_msgs[this->spi_monitor_index]->num_rx_bytes = character - '0';
            } else {
                this->spi_cmd_msg->num_rx_bytes = character - '0';
            }
            UARTprintf("%c", character);
            UARTprintf("\nPress Spacebar to send:\n\r");
            this->cmd_state = SPI_SEND_MSG;
        }

        break;
    case SPI_SEND_MSG :

        if (this->monitored) {
            if (' ' == character) {
                UARTprintf("\nTab to next page to see monitored register\n");
                this->spi_monitor_msgs[this->spi_monitor_index]->active = true;
                this->cmd_state = SPI_GET_MONITOR_STATUS;
                this->add_spi_msg(this->spi_monitor_msgs[this->spi_monitor_index]);
                this->spi_monitor_index++;
                this->monitored = false;
            }
        } else {
            if (' ' == character) {
                this->add_spi_msg(this->spi_cmd_msg);
                this->cmd_state = SPI_GET_MONITOR_STATUS;
                this->monitored = false;

            }
        }

        break;
    default :
        assert(0);
        break;
    }

}

void SpiCmdTask::draw_reset(void) {

    this->cmd_state = SPI_GET_MONITOR_STATUS;
    this->byte_counter = 0;
    this->byte_buffer_index = 0;
    this->byte_buffer = 0;
    UARTprintf("\n\rMonitor register? y/n :");
}

void SpiCmdTask::draw_help(void) {

}
