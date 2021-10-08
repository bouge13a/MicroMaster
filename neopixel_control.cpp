/*
 * neopixel_control.cpp
 *
 *  Created on: Oct 8, 2021
 *      Author: steph
 */

#include "neopixel_control.hpp"


#include <console_uart.hpp>
#include "uartstdio.h"
#include "neopixel_control.hpp"
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

static void timer4_int_handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

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

NeopixMsg::NeopixMsg(void) {

} // End NeopixMsg::NeopixMsg

void NeopixelCtl::taskfunwrapper(void* parm){
    (static_cast<NeopixelCtl*>(parm))->task((NeopixelCtl*)parm);
} // End NeopixelCtl::taskfunwrapper


NeopixelCtl::NeopixelCtl(GpoObj* gpo_object) : ConsolePage("NeoPixel Control",
                                                     portMAX_DELAY,
                                                     false) {

    this->gpo_object = gpo_object;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Neopixel Ctl",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->neopix_msg_q = xQueueCreate(2, sizeof(uint32_t*));

    timer_semphr = xSemaphoreCreateBinary();

    this->data_pin = gpo_object->get_config("GPO 3");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4));

    TimerClockSourceSet(TIMER4_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER4_BASE, TIMER_A);
    TimerConfigure(TIMER4_BASE, TIMER_CFG_ONE_SHOT);

    IntEnable(INT_TIMER4A);

    TimerIntRegister(TIMER4_BASE, TIMER_A, timer4_int_handler);
    IntPrioritySet(INT_TIMER4A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

} // End NeopixelCtl


void NeopixelCtl::task(NeopixelCtl* this_ptr) {

    while(1){

        switch(this_ptr->neopix_state) {
        case NEOPIX_IDLE :

            xQueueReceive(this_ptr->neopix_msg_q, &this_ptr->neopix_msg, portMAX_DELAY);

            assert(this_ptr->neopix_msg);



            break;

        default :
            assert(0);
            break;
        }

    }

} // End AdcTask::task

void NeopixelCtl::draw_page(void) {

}
void NeopixelCtl::draw_data(void) {

}
void NeopixelCtl::draw_input(int character) {


}

void NeopixelCtl::draw_reset(void) {

}

void NeopixelCtl::draw_help(void) {


}
