

/**
 * main.cpp
 * Author: Stephan Bourgeois 7/12/2021
 */

#include <stdint.h>
#include <assert.h>


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "debug.h"
#include "driverlib/fpu.h"
#include "driverlib/rom_map.h"
#include "sysctl.h"
#include "driverlib/interrupt.h"

#include "no_booster_board.hpp"

int main(void) {

    // initialize tiva-c @ 80mhz
    MAP_FPUEnable();
    MAP_FPULazyStackingEnable();
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    NoBoosterPack* no_booster = new NoBoosterPack();


    // Enable processor interrupts.
    IntMasterEnable();

    ///////////////////////////////////////////////////////
    //               Start Scheduler
    ///////////////////////////////////////////////////////
    vTaskStartScheduler();

}
