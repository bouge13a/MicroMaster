/*
 * CAN_command.cpp
 *
 *  Created on: Sep 1, 2021
 *      Author: steph
 */


#include "CAN_command.hpp"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "uartstdio.h"
#include "text_controls.hpp"

static void CANIntHandler(void) {

    uint32_t ui32Status;

    // Read the CAN interrupt status to find the cause of the interrupt
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);


} // End CANIntHandler

CanCommand::CanCommand(void) : ConsolePage("CAN Command",
                                           portMAX_DELAY,
                                           false) {

    xTaskCreate(this->tx_taskfunwrapper, /* Function that implements the task. */
                "CAN TX",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    xTaskCreate(this->rx_taskfunwrapper, /* Function that implements the task. */
                "CAN RX",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->can_tx_q = xQueueCreate(2, sizeof(uint32_t*));


    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);

    CANIntRegister(CAN0_BASE, CANIntHandler); // if using dynamic vectors

    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    IntEnable(INT_CAN0);

    CANEnable(CAN0_BASE);

    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    can_rx_msg.ui32MsgID = 0;
    can_rx_msg.ui32MsgIDMask = 0;
    can_rx_msg.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    can_rx_msg.ui32MsgLen = 8;

    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object 1 for receiving messages (this is not the same as
    // the CAN ID which can be any value in this example).
    CANMessageSet(CAN0_BASE, 1, &can_rx_msg, MSG_OBJ_TYPE_RX);

} // End TestTask

void CanCommand::tx_taskfunwrapper(void* parm){
    (static_cast<CanCommand*>(parm))->tx_task((CanCommand*)parm);
} // End CanCommand::taskfunwrapper

void CanCommand::rx_taskfunwrapper(void* parm){
    (static_cast<CanCommand*>(parm))->rx_task((CanCommand*)parm);
} // End CanCommand::taskfunwrapper

void CanCommand::tx_task(CanCommand* this_ptr) {

    while(1){

        vTaskDelay(1000);
    }

} // End AdcTask::task

void CanCommand::rx_task(CanCommand* this_ptr) {

    while(1){

        vTaskDelay(1000);
    }

} // End AdcTask::task

void CanCommand::draw_page(void) {

}
void CanCommand::draw_data(void) {

}
void CanCommand::draw_input(int character) {

}

void CanCommand::draw_reset(void) {

}

void CanCommand::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
