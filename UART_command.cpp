/*
 * UART_command.cpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */


#include "UART_command.hpp"

UartCmd::UartCmd(void)  : ConsolePage("UART Command",
                                              portMAX_DELAY,
                                              false){


} // End UartCmdTask::UartCmdTask


void UartCmd::draw_page(void) {

}
void UartCmd::draw_data(void) {

}
void UartCmd::draw_input(int character) {

}

void UartCmd::draw_reset(void) {

}

void UartCmd::draw_help(void) {

}
