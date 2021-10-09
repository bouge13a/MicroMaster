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
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
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


NeopixelCtl::NeopixelCtl(void) : ConsolePage("NeoPixel Control",
                                                     portMAX_DELAY,
                                                     false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Neopixel Ctl",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->neopix_msg_q = xQueueCreate(2, sizeof(uint32_t*));

    timer_semphr = xSemaphoreCreateBinary();



    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4));

    TimerClockSourceSet(TIMER4_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER4_BASE, TIMER_A);
    TimerConfigure(TIMER4_BASE, TIMER_CFG_ONE_SHOT);

    IntEnable(INT_TIMER4A);

    TimerIntRegister(TIMER4_BASE, TIMER_A, timer4_int_handler);
    IntPrioritySet(INT_TIMER4A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
    GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    GPIOPinConfigure(GPIO_PF3_SSI1FSS);

    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                                    GPIO_PIN_3);

    SSIConfigSetExpClk(SSI1_BASE,
                       SysCtlClockGet(),
                       SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER,
                       2400000,
                       8);

    SSIEnable(SSI1_BASE);

} // End NeopixelCtl


void NeopixelCtl::task(NeopixelCtl* this_ptr) {

    while(1){

        switch(this_ptr->neopix_state) {
        case NEOPIX_IDLE :

            xQueueReceive(this_ptr->neopix_msg_q, &this_ptr->neopix_msg, portMAX_DELAY);

            assert(this_ptr->neopix_msg);



            break;

        case NEOPIX_SEND :

            break;

        case NEOPIX_FINISH :

            break;

        default :
            assert(0);
            break;
        }

    }

} // End AdcTask::task

void NeopixelCtl::set_timer(uint32_t useconds) {

    TimerDisable(TIMER4_BASE, TIMER_A);
    TimerLoadSet(TIMER4_BASE, TIMER_A, useconds*80);
    TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER4_BASE, TIMER_A);

} // End OneWireCmd::set_timer

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
