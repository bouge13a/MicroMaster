/*
 * UART_streamer.cpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */

#include "UART_streamer.hpp"
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

static QueueHandle_t uart_rx_queue = NULL;

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

UartStreamer::UartStreamer(void)  : ConsolePage("UART Streamer",
                                                portMAX_DELAY,
                                                false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "UART Streamer",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    uart_rx_queue =  xQueueCreate(20, sizeof(char));

    // Enable the peripherals used by UART
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Set GPIO B0 and B1 as UART pins.
    MAP_GPIOPinConfigure(GPIO_PB0_U1RX);
    MAP_GPIOPinConfigure(GPIO_PB1_U1TX);

    MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    MAP_UARTClockSourceSet(UART1_BASE, UART_CLOCK_SYSTEM);

    UARTConfigSetExpClk(UART1_BASE,
                        MAP_SysCtlClockGet(),
                        115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOEnable(UART1_BASE);

    // Enable RX interrupt only
    MAP_IntEnable(INT_UART1);
    MAP_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

    MAP_IntPrioritySet(INT_UART1, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    // Register the interrupt
    UARTIntRegister(UART1_BASE, UART1_int_handler);


} // End UartStreamer::UartStreamer

void UartStreamer::taskfunwrapper(void* parm){
    (static_cast<UartStreamer*>(parm))->task((UartStreamer*)parm);
} // End UartStreamer::taskfunwrapper

void UartStreamer::task(UartStreamer* this_ptr) {

    unsigned char character = 0;

    while(1) {

        xQueueReceive(uart_rx_queue, &character, portMAX_DELAY);

        if (this_ptr->on_screen) {
            if('\r' == character) {
                UARTprintf("\n\r");
            } else if (character < ' ') {
                TextCtl::text_color(TextCtl::cyan_text);
                UARTprintf("[%d]", character);
                TextCtl::text_color(TextCtl::white_text);
            } else {
                UARTprintf("%c", character);
            }

        }
    }
} // End UartStreamer::task

void UartStreamer::draw_page(void) {

    UARTprintf("Incoming UART messages will stream\r\n\n");

}
void UartStreamer::draw_data(void) {

}
void UartStreamer::draw_input(int character) {

}

void UartStreamer::draw_reset(void) {

}

void UartStreamer::draw_help(void) {

}
