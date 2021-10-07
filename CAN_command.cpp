/*
 * CAN_command.cpp
 *
 *  Created on: Sep 1, 2021
 *      Author: steph
 */

#include <assert.h>

#include "CAN_command.hpp"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "uartstdio.h"
#include "text_controls.hpp"

static volatile uint32_t ui32Status;

static volatile bool error_flag = false;

static SemaphoreHandle_t can_rx_semphr;

static const uint32_t SIZE_OF_RX_MSG_DATA = 8;

static const uint32_t MAX_FREQ = 1000000;
static const uint32_t MIN_FREQ = 1000;

static void CANIntHandler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    // Read the CAN interrupt status to find the cause of the interrupt
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    if(ui32Status == (ui32Status & CAN_INT_INTID_STATUS)) {

        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        error_flag = true;

    } else if (ui32Status == CAN_RX_MESSAGE_OBJ) {

        CANIntClear(CAN0_BASE, CAN_RX_MESSAGE_OBJ);
        error_flag = false;

        xResult = xSemaphoreGiveFromISR( can_rx_semphr, &xHigherPriorityTaskWoken );

        // Was the message posted successfully?
        if( xResult != pdFAIL ) {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }

    } else if (ui32Status == CAN_TX_MESSAGE_OBJ) {

        CANIntClear(CAN0_BASE, CAN_TX_MESSAGE_OBJ);
        error_flag = false;

    } else {

        assert(0);
    }

} // End CANIntHandler

CanCommand::CanCommand(QueueHandle_t can_rx_q) : ConsolePage("CAN Command",
                                                              portMAX_DELAY,
                                                              false) {

    this->can_rx_q = can_rx_q;

    xTaskCreate(this->tx_taskfunwrapper, /* Function that implements the task. */
                "CAN TX",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    xTaskCreate(this->rx_taskfunwrapper, /* Function that implements the task. */
                "CAN RX",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->can_tx_q = xQueueCreate(2, sizeof(uint32_t*));

    can_rx_semphr = xSemaphoreCreateBinary();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0));

    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, 80000000, 500000);

    CANIntRegister(CAN0_BASE, CANIntHandler); // if using dynamic vectors

    IntPrioritySet(INT_CAN0, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    IntEnable(INT_CAN0);

    CANEnable(CAN0_BASE);

    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID.  In order to receive any CAN ID, the ID and mask must both
    // be set to 0, and the ID filter enabled.
    this->can_rx_msg.ui32MsgID     = 0;
    this->can_rx_msg.ui32MsgIDMask = 0;
    this->can_rx_msg.ui32Flags     = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    this->can_rx_msg.ui32MsgLen    = SIZE_OF_RX_MSG_DATA;
    this->can_rx_msg.pui8MsgData   = new uint8_t[SIZE_OF_RX_MSG_DATA];

    this->can_tx_msg.ui32Flags     = MSG_OBJ_TX_INT_ENABLE;
    this->can_tx_msg.ui32MsgIDMask = 0;
    this->can_tx_msg.pui8MsgData   = new uint8_t[SIZE_OF_RX_MSG_DATA];

    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object 1 for receiving messages (this is not the same as
    // the CAN ID which can be any value in this example).
    CANMessageSet(CAN0_BASE, CAN_RX_MESSAGE_OBJ, &can_rx_msg, MSG_OBJ_TYPE_RX);

    this->logger = ErrorLogger::get_instance();

    this->bus_off_err     = this->logger->create_error("CAN0", "Entered a Bus Off state");
    this->ewarn_err       = this->logger->create_error("CAN0", "Error level has reached warning level");
    this->epass_err       = this->logger->create_error("CAN0", "Error level has reached error passive level");
    this->lec_stuff_err   = this->logger->create_error("CAN0", "A bit stuffing error has occurred");
    this->lec_form_err    = this->logger->create_error("CAN0", "A formatting error has occurred");
    this->lec_ack_err     = this->logger->create_error("CAN0", "An acknowledge error has occurred");
    this->lec_bit1_err    = this->logger->create_error("CAN0", "The bus remained at a bit level of 1");
    this->lec_bit0_err    = this->logger->create_error("CAN0", "The bus remained at a bit level of 0");
    this->lec_crc_err     = this->logger->create_error("CAN0", "A CRC error has occurred");

    this->can_cmd_state = CAN_CMD_SPEED;
    this->byte_buffer = 0;
    this->byte_buffer_idx = 0;
    this->byte_counter = 0;

    this->msg_rdy_flag = false;

    this->speed_buffer = new uint8_t[10];

} // End TestTask

void CanCommand::send_last_message(void) {

    if(this->msg_rdy_flag == true) {
        xQueueSend(this->can_tx_q, &this->can_tx_msg, 5);
    }

} // End CanCommand::send_last_message

bool CanCommand::add_can_msg(tCANMsgObject* can_tx_msg_p) {

    return xQueueSend(this->can_tx_q, &can_tx_msg_p, 0);
}

void CanCommand::tx_taskfunwrapper(void* parm){
    (static_cast<CanCommand*>(parm))->tx_task((CanCommand*)parm);
} // End CanCommand::taskfunwrapper

void CanCommand::rx_taskfunwrapper(void* parm){
    (static_cast<CanCommand*>(parm))->rx_task((CanCommand*)parm);
} // End CanCommand::taskfunwrapper

void CanCommand::log_print_errors(void) {

    if (CAN_STATUS_BUS_OFF == (CAN_STATUS_BUS_OFF & ui32Status)) {
        this->logger->set_error(this->bus_off_err);
        UARTprintf("\r\nError: %s", this->bus_off_err->info);
    }

    if (CAN_STATUS_EWARN == (CAN_STATUS_EWARN & ui32Status)) {
        this->logger->set_error(this->ewarn_err);
        UARTprintf("\r\nError: %s", this->ewarn_err->info);
    }

    if (CAN_STATUS_EPASS == (CAN_STATUS_EPASS & ui32Status)) {
        this->logger->set_error(this->epass_err);
        UARTprintf("\r\nError: %s", this->epass_err->info);
    }

    if (CAN_STATUS_LEC_STUFF == (CAN_STATUS_LEC_STUFF & ui32Status)) {
        this->logger->set_error(this->lec_stuff_err);
        UARTprintf("\r\nError: %s", this->lec_stuff_err->info);
    }

    if (CAN_STATUS_LEC_FORM == (CAN_STATUS_LEC_FORM & ui32Status)) {
        this->logger->set_error(this->lec_form_err);
        UARTprintf("\r\nError: %s", this->lec_form_err->info);
    }

    if (CAN_STATUS_LEC_ACK == (CAN_STATUS_LEC_ACK & ui32Status)) {
        this->logger->set_error(this->lec_ack_err);
        UARTprintf("\r\nError: %s", this->lec_ack_err->info);
    }

    if (CAN_STATUS_LEC_BIT1 == (CAN_STATUS_LEC_BIT1 & ui32Status)) {
        this->logger->set_error(this->lec_bit1_err);
        UARTprintf("\r\nError: %s", this->lec_bit1_err->info);
    }

    if (CAN_STATUS_LEC_BIT0 == (CAN_STATUS_LEC_BIT0 & ui32Status)) {
        this->logger->set_error(this->lec_bit0_err);
        UARTprintf("\r\nError: %s", this->lec_bit0_err->info);
    }

    if (CAN_STATUS_LEC_CRC == (CAN_STATUS_LEC_CRC & ui32Status)) {
        this->logger->set_error(this->lec_crc_err);
        UARTprintf("\r\nError: %s", this->lec_crc_err->info);
    }

} // End CanCommand::log_print_errors

void CanCommand::tx_task(CanCommand* this_ptr) {

    while(1){

        xQueueReceive(this_ptr->can_tx_q, &this_ptr->can_tx_msg_p, portMAX_DELAY);

        CANMessageSet(CAN0_BASE, CAN_TX_MESSAGE_OBJ, this_ptr->can_tx_msg_p, MSG_OBJ_TYPE_TX);

        vTaskDelay(10);

        if (this_ptr->on_screen) {
            if(error_flag) {
                this_ptr->log_print_errors();
                error_flag = false;
            } else {
                UARTprintf("\r\nMessage transmitted successfully");

            }

            UARTprintf("\r\n\nEnter CAN bus speed (1000-1000000Hz) : ");
        }

        vTaskDelay(0);
    }

} // End AdcTask::task

void CanCommand::send_can_message(CanCommand* this_ptr, tCANMsgObject* can_rx_msg_p) {
    xQueueSend(this_ptr->can_rx_q, &can_rx_msg_p, 0);
}

void CanCommand::rx_task(CanCommand* this_ptr) {

    while(1){

        xSemaphoreTake(can_rx_semphr, portMAX_DELAY);

        CANMessageGet(CAN0_BASE, CAN_RX_MESSAGE_OBJ, &this_ptr->can_rx_msg, true);

        send_can_message(this_ptr, &this_ptr->can_rx_msg);

        error_flag = false;

        vTaskDelay(0);
    }

} // End AdcTask::task

static uint32_t ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
} // End SpiCmdTask::ascii_to_hex

void CanCommand::draw_page(void) {

    UARTprintf("Enter CAN bus speed (1000-1000000Hz) : ");

}
void CanCommand::draw_data(void) {

}
void CanCommand::draw_input(int character) {

    switch (this->can_cmd_state) {
    case CAN_CMD_SPEED :

        if (character == 127) {
            if(this->byte_buffer_idx > 0) {
                this->byte_buffer_idx--;
                UARTprintf("\b");
                TextCtl::clear_in_line();
                return;
            }
        }

        if ((character >= '0') && (character <= '9')) {

            this->speed_buffer[this->byte_buffer_idx] = (uint8_t)character;
            this->byte_buffer_idx++;
            UARTprintf("%c", (uint8_t)character);
        } else if (character == '\r') {
            UARTprintf("%c", (uint8_t)character);
            this->speed_buffer[this->byte_buffer_idx] = '\0';

            if(atoi((const char*)this->speed_buffer) > MAX_FREQ) {
                this->byte_buffer_idx = 0;
                UARTprintf("\r\nError: freqency must be less than 1MHz\r\n");
                UARTprintf("Enter CAN bus speed (1000-1000000Hz) : ");
                break;
            }

            if(atoi((const char*)this->speed_buffer) < MIN_FREQ) {
                this->byte_buffer_idx = 0;
                UARTprintf("\r\nError: freqency must be greater than 1KHz\r\n");
                UARTprintf("Enter CAN bus speed (1000-1000000Hz) : ");
                break;
            }

            CANBitRateSet(CAN0_BASE, 80000000, atoi((const char*)this->speed_buffer));

            this->byte_buffer_idx = 0;
            this->can_cmd_state = CAN_CMD_ID;

            UARTprintf("\r\nEnter 29 bit CAN ID : 0x");
        } else {
            this->send_bell();
        }


        break;
    case CAN_CMD_ID :
        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){
            if (this->byte_buffer_idx == 0) {

                this->byte_buffer = ascii_to_hex(character) << 4;
                this->byte_buffer_idx++;
                UARTprintf("%c", character);

            } else if (this->byte_buffer_idx == 1) {

                this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                this->byte_buffer_idx = 0;
                this->can_tx_msg.ui32MsgID |= (this->byte_buffer << ((1-this->byte_counter) * 8));
                this->byte_counter++;
                UARTprintf("%c", character);

                if (this->byte_counter >= 2) {

                    this->can_cmd_state = CAN_CMD_NUM_TX_BYTES;
                    this->byte_counter = 0;
                    UARTprintf("\r\nEnter number of TX bytes (1-8) : ");

                }
            }
        } else {
            this->send_bell();
        }
        break;

    case CAN_CMD_NUM_TX_BYTES :

        if ((character >= '0' && character <= '8')) {

            this->can_tx_msg.ui32MsgLen = character - '0';
            UARTprintf("%c", character);
            UARTprintf("\r\nbyte 1 : 0x");
            this->can_cmd_state = CAN_CMD_TX_BYTES;

        } else {
            this->send_bell();
        }

        break;

    case CAN_CMD_TX_BYTES :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (0 == this->byte_buffer_idx) {

                this->byte_buffer = ascii_to_hex(character) << 4;
                this->byte_buffer_idx++;
                UARTprintf("%c", character);

            } else {

                this->byte_buffer = this->byte_buffer | ascii_to_hex(character);
                this->can_tx_msg.pui8MsgData[this->byte_counter] = this->byte_buffer;
                this->byte_buffer_idx = 0;
                this->byte_counter++;
                UARTprintf("%c", character);

                if(this->byte_counter < this->can_tx_msg.ui32MsgLen) {
                    UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                }
            }

            if (this->byte_counter >= this->can_tx_msg.ui32MsgLen ) {
                this->byte_counter = 0;
                this->can_cmd_state = CAN_CMD_SEND;
                this->byte_buffer_idx = 0;
                UARTprintf("\n\rPress space-bar to send message or s to save message : ");
            }
        } else {
            this->send_bell();
        }

        break;

    case CAN_CMD_SEND :
        if (' ' == character) {
            add_can_msg(&this->can_tx_msg);
            this->msg_rdy_flag = true;
        } else if ('s' == character) {
            this->msg_rdy_flag = true;
            UARTprintf("\r\n\nEnter CAN bus speed (1000-1000000Hz) : ");
        } else {
            this->send_bell();
        }

        this->can_cmd_state = CAN_CMD_SPEED;
        break;

    default :
        assert(0);
        break;

    }

}

void CanCommand::draw_reset(void) {

    this->byte_buffer_idx = 0;
    this->byte_counter = 0;
    this->can_cmd_state = CAN_CMD_SPEED;
    UARTprintf("\r\n\nEnter 29 bit CAN ID : 0x");

}

void CanCommand::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
