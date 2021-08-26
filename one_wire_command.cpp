/*
 * one_wire_command.cpp
 *
 *  Created on: Aug 26, 2021
 *      Author: steph
 */

#include <assert.h>

#include "uartstdio.h"
#include "one_wire_command.hpp"
#include "text_controls.hpp"

#include "driverlib/inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"


static SemaphoreHandle_t timer_semphr = NULL;

static const uint32_t NUM_OF_TX_MSGS = 10;
static const uint32_t NUM_OF_RX_MSGS = 10;

static void timer0_int_handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    xResult = xSemaphoreGiveFromISR( timer_semphr, &xHigherPriorityTaskWoken );

    // Was the message posted successfully?
    if( xResult != pdFAIL ) {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

} // End void timer0_int_handler

OneWireMsg::OneWireMsg(void) {

    this->bytes_rxed = 0;
    this->bytes_txed = 0;
    this->num_rx_bytes = 0;
    this->num_tx_bytes = 0;

} // End OneWireMsg::OneWireMsg

void OneWireCmd::taskfunwrapper(void* parm){
    (static_cast<OneWireCmd*>(parm))->task((OneWireCmd*)parm);
} // End OneWireCmd::taskfunwrapper


OneWireCmd::OneWireCmd(GpoObj* gpo_obj) : ConsolePage("1 Wire Command",
                                           portMAX_DELAY,
                                           false) {

    this->gpo_obj = gpo_obj;
    this->one_wire_pin = gpo_obj->get_config("GPO 3");

    xTaskCreate(this->taskfunwrapper,     /* Function that implements the task. */
                "1 wire cmd",             /* Text name for the task. */
                100,                      /* Stack size in words, not bytes. */
                this,                     /* Parameter passed into the task. */
                3,                        /* Priority at which the task is created. */
                NULL);

    timer_semphr = xSemaphoreCreateBinary();

    this->one_wire_q = xQueueCreate(2, sizeof(uint32_t*));

    this->one_wire_cmd_msg = new OneWireMsg();
    this->one_wire_cmd_msg->tx_bytes = new uint8_t[NUM_OF_TX_MSGS];
    this->one_wire_cmd_msg->rx_bytes = new uint8_t[NUM_OF_RX_MSGS];

    this->one_wire_state = ONE_WIRE_IDLE;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER0_BASE, TIMER_BOTH);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //TimerEnable(TIMER0_BASE, TIMER_BOTH);

    IntEnable(INT_TIMER0A);

    TimerIntRegister(TIMER0_BASE, TIMER_A, timer0_int_handler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntPrioritySet(INT_TIMER0A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

} // End TestTask


void OneWireCmd::task(OneWireCmd* this_ptr) {

    while(1){

        switch(this->one_wire_state) {

        case ONE_WIRE_IDLE :

            xQueueReceive(one_wire_q, &this->one_wire_msg, portMAX_DELAY);

            break;

        case ONE_WIRE_SEND :

            break;

        case ONE_WIRE_RECEIVE :

            break;

        case ONE_WIRE_FINISH :

            break;

        default :
            assert(0);
            break;


        }
        vTaskDelay(1000);

    }

} // End AdcTask::task

void OneWireCmd::draw_page(void) {

}
void OneWireCmd::draw_data(void) {

}
void OneWireCmd::draw_input(int character) {

}

void OneWireCmd::draw_reset(void) {

}

void OneWireCmd::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}


