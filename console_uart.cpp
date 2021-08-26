/*
 * uart_task.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */


#include <console_uart.hpp>
#include <stdint.h>

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

static QueueHandle_t uart_rx_q;

static void UART_RX_int_handler(void) {

    uint8_t character = 1;

    uint32_t ui32Status = 0;

    while (character != 0xFF) {
        character = UARTCharGetNonBlocking(UART0_BASE);
        if (character != 0xFF) {
            xQueueSendFromISR(uart_rx_q, &character, 0);
        }
    }

    ui32Status = UARTIntStatus(UART0_BASE, true);

    UARTIntClear(UART0_BASE, ui32Status);

} // End UART_RX_int_handler

UartTask::UartTask(QueueHandle_t uart_rx_queue) {

    // Set the UART rx queue
    uart_rx_q = uart_rx_queue;

    // Enable the peripherals used by UART
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    // Set GPIO A0 and A1 as UART pins.
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);

    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);

    // Enable RX interrupt only
    MAP_IntEnable(INT_UART0);
    MAP_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    MAP_IntPrioritySet(INT_UART0, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    // Register the interrupt
    UARTIntRegister(UART0_BASE, UART_RX_int_handler);

} // End UartTask::UartTask


