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
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

void SpiCmdTask::taskfunwrapper(void* parm){
    (static_cast<SpiCmdTask*>(parm))->task((SpiCmdTask*)parm);
} // End SpiCmdTask::taskfunwrapper

SpiCmdTask::SpiCmdTask(void) : ConsolePage("Test page",
                                           portMAX_DELAY,
                                           false)  {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Test",                                     /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);




} // End SpiCmdTask(void)

void SpiCmdTask::task(SpiCmdTask* this_ptr) {

    while(1) {

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
