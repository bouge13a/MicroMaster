

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

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    MAP_FPUEnable();
    MAP_FPULazyStackingEnable();


    // Set the system clock to 50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 |
                       SYSCTL_USE_PLL |
                       SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    NoBoosterPack* no_booster = new NoBoosterPack();


    // Enable processor interrupts.
    IntMasterEnable();

    ///////////////////////////////////////////////////////
    //               Start Scheduler
    ///////////////////////////////////////////////////////
    vTaskStartScheduler();

}
