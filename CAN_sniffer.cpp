/*
 * CAN_sniffer.cpp
 *
 *  Created on: Sep 2, 2021
 *      Author: steph
 */


#include "CAN_sniffer.hpp"

#include <console_uart.hpp>
#include "uartstdio.h"
#include "text_controls.hpp"

static const uint32_t MSG_ID_COL = 0;
static const uint32_t MSG_DATA_COL = 20;
static const uint32_t START_ROW = 5;

void CanSniffer::taskfunwrapper(void* parm){
    (static_cast<CanSniffer*>(parm))->task((CanSniffer*)parm);
} // End CanSniffer::taskfunwrapper


CanSniffer::CanSniffer(QueueHandle_t can_rx_q,
                       CanCommand* can_command) : ConsolePage("CAN Sniffer",
                                                              portMAX_DELAY,
                                                              false) {

    this->can_rx_q = can_rx_q;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "CAN Sniffer",                                     /* Text name for the task. */
                70,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->msg_idx = 0;


} // End CanSniffer


void CanSniffer::task(CanSniffer* this_ptr) {

    while(1){

        xQueueReceive(this_ptr->can_rx_q, &this_ptr->can_rx_msg, portMAX_DELAY);

        TextCtl::cursor_pos(START_ROW + this_ptr->msg_idx, MSG_ID_COL);
        UARTprintf("0x%04x", this_ptr->can_rx_msg->ui32MsgID);
        TextCtl::cursor_pos(START_ROW + this_ptr->msg_idx, MSG_DATA_COL);
        for(uint32_t idx=0; idx < this_ptr->can_rx_msg->ui32MsgLen; idx++) {
            UARTprintf("0x%02x ", this_ptr->can_rx_msg->pui8MsgData[idx]);
        }
    }
} // End CanSniffer::task

void CanSniffer::draw_page(void) {

    UARTprintf("Press space-bar to send last message\r\n");
    TextCtl::set_text_mode(TextCtl::mode_underline);
    TextCtl::cursor_pos(START_ROW, MSG_ID_COL);
    UARTprintf("MSG ID");
    TextCtl::cursor_pos(START_ROW, MSG_DATA_COL);
    UARTprintf("Message Data\r\n");
    TextCtl::set_text_mode(TextCtl::mode_underline_off);

} // End CanSniffer::draw_page

void CanSniffer::draw_data(void) {

} // End CanSniffer::draw_data

void CanSniffer::draw_input(int character) {

    if (' ' == character) {
        this->can_command->send_last_message();
        UARTprintf("\r\nMessage Transmitted");
        this->msg_idx++;
    }

} // End CanSniffer::draw_input

void CanSniffer::draw_reset(void) {

    this->msg_idx = 0;

} // End CanSniffer::draw_reset

void CanSniffer::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

} // End CanSniffer::draw_help
