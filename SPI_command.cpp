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

static uint32_t spi0_status = 0;

static void spi0_int_handler(void) {



}

SpiMsg::SpiMsg(void) {

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

    this->spi_tx_queue = xQueueCreate(20, sizeof(SpiMsg*));

    this->spi_msg = new SpiMsg();
    this->spi_msg->tx_bytes = new uint32_t[SPI_TX_Q_NUM];
    this->spi_msg->rx_bytes = new uint32_t[SPI_RX_Q_NUM];
    this->spi_msg->bytes_txed = 0;
    this->spi_msg->bytes_rxed = 0;
    this->spi_msg->state = spi_ready;

    while(SSIDataGetNonBlocking(SSI0_BASE, &this->spi_msg->rx_bytes[0]));

} // End SpiCmdTask(void)

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

            if (this_ptr->spi_msg->bytes_rxed < this_ptr->spi_msg->num_rx_bytes) {

                SSIDataGetNonBlocking(SSI0_BASE,
                                      &this->spi_msg->tx_bytes[this_ptr->spi_msg->bytes_rxed]);

                this_ptr->spi_msg->bytes_rxed++;

            } else {

                this_ptr->state = SPI_FINISH;

            }

            break;
        case SPI_FINISH :

            while(SSIBusy(SSI0_BASE)) {
                break;
            }

            this_ptr->spi_msg->state = spi_finished;
            this_ptr->state = SPI_IDLE;


            break;
        default :
            break;
        }
    }
}

void SpiCmdTask::draw_page(void) {


}

void SpiCmdTask::draw_data(void) {

}

void SpiCmdTask::draw_input(int character) {

}

void SpiCmdTask::draw_reset(void) {

}

void SpiCmdTask::draw_help(void) {

}
