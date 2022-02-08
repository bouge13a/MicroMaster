///*
// * I2C_sniffer.cpp
// *
// *  Created on: Sep 7, 2021
// *      Author: steph
// */
//
//
//#include <SPI_sniffer.hpp>
//#include "driverlib/inc/hw_gpio.h"
//#include "driverlib/inc/hw_types.h"
//#include "driverlib/inc/hw_memmap.h"
//#include "driverlib/gpio.h"
//#include "driverlib/pin_map.h"
//#include "driverlib/rom.h"
//#include "driverlib/rom_map.h"
//#include "driverlib/sysctl.h"
//#include "uartstdio.h"
//
//static void spi_tx_int_handler(void) {
//
//
//
//} // End spi_tx_int_handler
//
//static void spi_rx_int_handler(void) {
//
//
//
//} // End spi_rx_int_handler
//
//SpiSniffer::SpiSniffer(SpiCmdTask* spi_cmd) {
//
//    this->spi_cmd = spi_cmd;
//
//    xTaskCreate(this->tx_taskfunwrapper, /* Function that implements the task. */
//                "SPI TX",                                     /* Text name for the task. */
//                80,                  /* Stack size in words, not bytes. */
//                this,                                      /* Parameter passed into the task. */
//                3,                                         /* Priority at which the task is created. */
//                NULL);
//
//    xTaskCreate(this->rx_taskfunwrapper, /* Function that implements the task. */
//                "SPI RX",                                     /* Text name for the task. */
//                80,                  /* Stack size in words, not bytes. */
//                this,                                      /* Parameter passed into the task. */
//                3,                                         /* Priority at which the task is created. */
//                NULL);
//
//
//} // End SpiSniffer
//
//
//void SpiSniffer::tx_taskfunwrapper(void* parm){
//    (static_cast<SpiSniffer*>(parm))->task((SpiSniffer*)parm);
//} // End FtdiEmulator::taskfunwrapper
//
//void SpiSniffer::rx_taskfunwrapper(void* parm){
//    (static_cast<SpiSniffer*>(parm))->task((SpiSniffer*)parm);
//} // End FtdiEmulator::taskfunwrapper
//
//void SpiSniffer::tx_task(SpiSniffer* this_ptr) {
//
//    while(1) {
//
//    }
//
//} // End SpiSniffer::tx_task
//
//void SpiSniffer::rx_task(SpiSniffer* this_ptr) {
//
//    while(1) {
//
//    }
//
//} // End SpiSniffer::tx_task
//
//void SpiSniffer::init_spi_sniffer(void) {
//
//    ////////////////////////////////////////////////////
//    //      TX SPI
//    ////////////////////////////////////////////////////
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
//    SSIIntRegister(SSI1_BASE, spi_tx_int_handler);
//    SSIIntEnable(SSI1_BASE, SSI_RXTO | SSI_RXOR);
//
//    SSIEnable(SSI1_BASE);
//
//    SSIDisable(SSI0_BASE);
//
//    SSIIntRegister(SSI1_BASE, spi_rx_int_handler);
//    SSIIntEnable(SSI1_BASE, SSI_RXTO | SSI_RXOR);
//
//    SSIEnable(SSI0_BASE);
//}
//
//void SpiSniffer::draw_page(void) {
//
//
//}
//
//void SpiSniffer::draw_data(void) {
//
//}
//
//void SpiSniffer::draw_input(int character) {
//
//}
//
//void SpiSniffer::draw_reset(void) {
//
//}
//
//void SpiSniffer::draw_help(void) {
//
//    UARTprintf("THIS IS A HELP PAGE\r\n");
//
//}

