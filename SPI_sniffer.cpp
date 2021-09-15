/*
 * I2C_sniffer.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: steph
 */


#include <SPI_sniffer.hpp>
#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "uartstdio.h"


SpiSniffer::SpiSniffer(SpiCmdTask* spi_cmd) : ConsolePage("SPI sniffer",
                                            portMAX_DELAY,
                                            false) {

//    this->spi_cmd = spi_cmd;
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
//
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1));
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
//
//    GPIOPinConfigure(GPIO_PF0_SSI1RX);
//    GPIOPinConfigure(GPIO_PF1_SSI1TX);
//    GPIOPinConfigure(GPIO_PF2_SSI1CLK);
//    GPIOPinConfigure(GPIO_PF3_SSI1FSS);
//
//    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
//                   GPIO_PIN_3);
//
//    SSIConfigSetExpClk(SSI1_BASE,
//                       SysCtlClockGet(),
//                       this->spi_cmd->get_mode(),
//                       SSI_MODE_MASTER,
//                       this->spi_cmd->get_speed(),
//                       this->spi_cmd->get_data_width());
//
//    SSIIntRegister(SSI1_BASE, spi1_int_handler);
//    SSIIntEnable(SSI0_BASE, SSI_RXTO | SSI_RXOR);
//
//    SSIEnable(SSI0_BASE);


} // End SpiSniffer


void SpiSniffer::draw_page(void) {


}
void SpiSniffer::draw_data(void) {

}
void SpiSniffer::draw_input(int character) {



}

void SpiSniffer::draw_reset(void) {

}

void SpiSniffer::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
