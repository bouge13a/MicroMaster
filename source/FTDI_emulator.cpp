/*
 * FTDI_emulator.cpp
 *
 *  Created on: Oct 5, 2021
 *      Author: steph
 */


#include <console_uart.hpp>
#include "uartstdio.h"
#include "FTDI_emulator.hpp"
#include "text_controls.hpp"

#include "rom.h"
#include "rom_map.h"
#include "sysctl.h"
#include "gpio.h"
#include "uart.h"
#include "pin_map.h"
#include "interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "uartstdio.h"

QueueHandle_t uart_rx_queue = NULL;

static void UART1_int_handler(void) {

    uint8_t character = 1;

    uint32_t ui32Status = 0;

    while (character != 0xFF) {
        character = UARTCharGetNonBlocking(UART1_BASE);
        if (character != 0xFF) {
            xQueueSendFromISR(uart_rx_queue, &character, 0);
        }
    }

    ui32Status = UARTIntStatus(UART1_BASE, true);

    UARTIntClear(UART1_BASE, ui32Status);
}

void FtdiEmulator::taskfunwrapper(void* parm){
    (static_cast<FtdiEmulator*>(parm))->task((FtdiEmulator*)parm);
} // End FtdiEmulator::taskfunwrapper


FtdiEmulator::FtdiEmulator(void) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Test",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    uart_rx_queue =  xQueueCreate(20, sizeof(char));

    // Enable the peripherals used by UART
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));

    // Set GPIO B0 and B1 as UART pins.
    MAP_GPIOPinConfigure(GPIO_PC4_U1RX);
    MAP_GPIOPinConfigure(GPIO_PC5_U1TX);

    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Use the internal 16MHz oscillator as the UART clock source.
    MAP_UARTClockSourceSet(UART1_BASE, UART_CLOCK_SYSTEM);

    UARTConfigSetExpClk(UART1_BASE,
                        80000000,
                        115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOEnable(UART1_BASE);

    // Enable RX interrupt only
    MAP_IntEnable(INT_UART1);
    MAP_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

    MAP_IntPrioritySet(INT_UART1, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    // Register the interrupt
    UARTIntRegister(UART1_BASE, UART1_int_handler);

} // End FtdiEmulator


void FtdiEmulator::task(FtdiEmulator* this_ptr) {

    uint8_t rx_char = 0;

    while(1){

        xQueueReceive(uart_rx_queue, &rx_char, portMAX_DELAY);

        if (rx_char == '\n') {
            UARTprintf("\r\n");
        } else {
            UARTprintf("%c", rx_char);
        }

    }

} // End FtdiEmulator::task



