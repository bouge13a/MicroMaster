/*
 * one_wire_command.cpp
 *
 *  Created on: Aug 26, 2021
 *      Author: steph
 */

#include <assert.h>

#include "uartstdio.h"
#include "one_wire_command.hpp"
#include "text_controls.hpp"

#include "driverlib/inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"


static SemaphoreHandle_t timer_semphr = NULL;

static const uint32_t NUM_OF_TX_MSGS = 20;
static const uint32_t NUM_OF_RX_MSGS = 20;

static const uint32_t RESET_PULSE_TIME_US = 480;
static const uint32_t IDLE_TIME_US = 65;
static const uint32_t AFTER_RESET_WAIT_US = 480;
static const uint32_t START_BIT_TIME_US = 100;
static const uint32_t RELEASE_TIME_US = 65;
static const uint32_t INTER_BIT_TIME_US = 5;
static const uint32_t INTER_BYTE_TIME_US = 500;

static void timer0_int_handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    xResult = xSemaphoreGiveFromISR( timer_semphr, &xHigherPriorityTaskWoken );

    // Was the message posted successfully?
    if( xResult != pdFAIL ) {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

} // End void timer0_int_handler

OneWireMsg::OneWireMsg(void) {

    this->bytes_rxed = 0;
    this->bytes_txed = 0;
    this->num_rx_bytes = 0;
    this->num_tx_bytes = 0;

} // End OneWireMsg::OneWireMsg

void OneWireCmd::taskfunwrapper(void* parm){
    (static_cast<OneWireCmd*>(parm))->task((OneWireCmd*)parm);
} // End OneWireCmd::taskfunwrapper


OneWireCmd::OneWireCmd(GpoObj* gpo_obj) : ConsolePage("1 Wire Command",
                                           portMAX_DELAY,
                                           false) {

    this->gpo_obj = gpo_obj;
    this->one_wire_pin = gpo_obj->get_config("GPO 3");

    xTaskCreate(this->taskfunwrapper,     /* Function that implements the task. */
                "1 Wire Cmd",             /* Text name for the task. */
                100,                      /* Stack size in words, not bytes. */
                this,                     /* Parameter passed into the task. */
                3,                        /* Priority at which the task is created. */
                NULL);

    timer_semphr = xSemaphoreCreateBinary();

    this->one_wire_q = xQueueCreate(2, sizeof(uint32_t*));

    this->one_wire_cmd_msg = new OneWireMsg();
    this->one_wire_cmd_msg->tx_bytes = new uint8_t[NUM_OF_TX_MSGS];
    this->one_wire_cmd_msg->rx_bytes = new uint8_t[NUM_OF_RX_MSGS];

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    IntEnable(INT_TIMER0A);

    TimerIntRegister(TIMER0_BASE, TIMER_A, timer0_int_handler);
    IntPrioritySet(INT_TIMER0A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    this->logger = ErrorLogger::get_instance();
    this->pullup_err = this->logger->create_error("1 Wire", "Line state low, check pull up resistor");
    this->no_resp_err = this->logger->create_error("1 Wire", "No response from device");

    this->byte_buffer = new uint8_t[3];
    this->bit_counter = 0;
    this->byte_counter = 0;
    this->byte_buffer_index = 0;
    this->one_wire_write_state = ONE_WIRE_START;
    this->one_wire_cmd_state = ENTER_NUM_TX_BYTES;
    this->one_wire_state = ONE_WIRE_IDLE;

    this->gpo_obj->set(this->one_wire_pin, 1);

} // End TestTask


void OneWireCmd::task(OneWireCmd* this_ptr) {

    while(1){

        switch(this_ptr->one_wire_state) {

        case ONE_WIRE_IDLE :

            xQueueReceive(this_ptr->one_wire_q, &this_ptr->one_wire_msg, portMAX_DELAY);

            assert(this_ptr->one_wire_msg);

            if( 0 == this_ptr->gpo_obj->get(this_ptr->one_wire_pin)) {
                this_ptr->logger->set_error(this_ptr->pullup_err);
                this_ptr->one_wire_state = ONE_WIRE_FINISH;
                this_ptr->one_wire_msg->errors = ONE_WIRE_PULLUPP_ERR;
                break;
            }

            this_ptr->one_wire_msg->bytes_rxed = 0;
            this_ptr->one_wire_msg->bytes_txed = 0;
            this_ptr->one_wire_msg->errors = ONE_WIRE_NO_ERR;

            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
            this_ptr->set_timer(RESET_PULSE_TIME_US);

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
            this_ptr->set_timer(IDLE_TIME_US);

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if(this_ptr->gpo_obj->get(this_ptr->one_wire_pin)) {

                this_ptr->logger->set_error(this_ptr->no_resp_err);
                this_ptr->one_wire_state = ONE_WIRE_FINISH;
                this_ptr->one_wire_msg->errors = ONE_WIRE_NO_RESP_ERR;

            } else {

                this_ptr->one_wire_state = ONE_WIRE_SEND;
                this_ptr->set_timer(AFTER_RESET_WAIT_US);

            }

            break;

        case ONE_WIRE_SEND :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->one_wire_msg->bytes_txed < this_ptr->one_wire_msg->num_tx_bytes) {

                if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                    SysCtlDelay(START_BIT_TIME_US);

                    this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                    if(1 == ((this_ptr->one_wire_msg->tx_bytes[this_ptr->one_wire_msg->bytes_txed] >> (this_ptr->bit_counter)) & 0x0001)) {
                        this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                    } else {
                        this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                    }
                    this_ptr->set_timer(RELEASE_TIME_US);

                } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                    this_ptr->bit_counter = (this_ptr->bit_counter + 1) % 8;
                    if (0 == this_ptr->bit_counter) {
                        this_ptr->one_wire_msg->bytes_txed++;
                        this_ptr->set_timer(INTER_BYTE_TIME_US);
                    } else {
                        this_ptr->set_timer(INTER_BIT_TIME_US);
                    }
                    this_ptr->one_wire_write_state = ONE_WIRE_START;
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

                }


            } else {

                if (this_ptr->one_wire_msg->num_rx_bytes > 0) {
                    this_ptr->one_wire_state = ONE_WIRE_RECEIVE;
                    this_ptr->set_timer(INTER_BIT_TIME_US);
                } else {
                    this_ptr->one_wire_state = ONE_WIRE_FINISH;
                }

            }

            break;

        case ONE_WIRE_RECEIVE :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->one_wire_msg->bytes_rxed < this_ptr->one_wire_msg->num_rx_bytes) {

                if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                    SysCtlDelay(START_BIT_TIME_US);

                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

                    SysCtlDelay(10);

                    this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                    this_ptr->one_wire_msg->rx_bytes[this_ptr->one_wire_msg->bytes_rxed] |= this_ptr->gpo_obj->get(one_wire_pin) << (this->bit_counter);
                    this->set_timer(INTER_BIT_TIME_US);


                } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                    this_ptr->bit_counter = (this_ptr->bit_counter + 1) % 8;

                    if (0 == this_ptr->bit_counter) {
                        this_ptr->one_wire_msg->bytes_rxed++;
                        this_ptr->set_timer(INTER_BYTE_TIME_US);
                    } else {
                        this_ptr->set_timer(INTER_BIT_TIME_US);
                    }
                    this_ptr->one_wire_write_state = ONE_WIRE_START;
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

                }

            } else {

                this_ptr->one_wire_state = ONE_WIRE_FINISH;

            }


            break;

        case ONE_WIRE_FINISH :

            if(this_ptr->on_screen) {

                this_ptr->print_errors(this_ptr);

                UARTprintf("TX: ");
                for (uint32_t index=0; index<this_ptr->one_wire_msg->bytes_txed; index++) {
                    UARTprintf("0x%2x ", this_ptr->one_wire_msg->tx_bytes[index]);
                }

                UARTprintf("\r\nRX: ");
                for (uint32_t index=0; index<this_ptr->one_wire_msg->bytes_rxed; index++) {
                    UARTprintf("0x%2x ", this_ptr->one_wire_msg->rx_bytes[index]);
                }

                UARTprintf("\r\n\nEnter number of TX bytes : ");
            }

            this->one_wire_state = ONE_WIRE_IDLE;

            break;

        default :
            assert(0);
            break;


        }
        vTaskDelay(0);

    }

} // End AdcTask::task

void OneWireCmd::add_msg(OneWireMsg* msg) {

    xQueueSend(this->one_wire_q, &msg, 0);

} // End OneWireCmd::add_msg

void OneWireCmd::print_errors(OneWireCmd* this_ptr) {

    switch(this_ptr->one_wire_msg->errors) {
    case ONE_WIRE_NO_ERR :
        UARTprintf("\r\nMessage transmitted successfully\r\n");
        break;
    case ONE_WIRE_PULLUPP_ERR :
        UARTprintf("\r\nError: Line state low, check pullups\r\n");
        break;
    case ONE_WIRE_NO_RESP_ERR :
        UARTprintf("\r\nError: No response from device\r\n");
        break;
    default :
        assert(0);
        break;
    }

} // End OneWireCmd::print_errors

void OneWireCmd::set_timer(uint32_t useconds) {

    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerLoadSet(TIMER0_BASE, TIMER_A, useconds*48);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

} // End OneWireCmd::set_timer

static uint32_t ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
}


void OneWireCmd::draw_page(void) {

    UARTprintf("\r\nEnter number of TX bytes : ");

}
void OneWireCmd::draw_data(void) {

}

void OneWireCmd::draw_input(int character) {

    switch(this->one_wire_cmd_state) {
    case ENTER_NUM_TX_BYTES :

        if ((character >= '0' && character <= '9') && (this->byte_buffer_index < 3)){

            this->byte_buffer[this->byte_buffer_index] = (uint8_t)character;
            this->byte_buffer_index++;
            UARTprintf("%c", (uint8_t)character);
        } else if ('\r' == character) {
            UARTprintf("%c", (uint8_t)character);
            this->byte_buffer[this->byte_buffer_index] = '\0';

            if(atoi((const char*)this->byte_buffer) > NUM_OF_TX_MSGS) {
                this->byte_buffer_index = 0;
                UARTprintf("\r\nError: Maximum of %d bytes\r\n", NUM_OF_TX_MSGS);
                UARTprintf("Enter number of TX bytes : ");
                break;
            }

            this->one_wire_cmd_msg->num_tx_bytes = atoi((const char*)this->byte_buffer);
            this->byte_buffer_index = 0;
            this->one_wire_cmd_state = ENTER_TX_BYTES;
            UARTprintf("\r\nbyte 1 : 0x");
        } else if(character >= '0' && character <= '9'){
            this->byte_buffer_index = 0;
            UARTprintf("\r\nError: Maximum of 2 digits\r\n");
            UARTprintf("Enter number of TX bytes : ");
        }

        break;
    case ENTER_TX_BYTES :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (0 == this->byte_buffer_index) {
                this->one_wire_cmd_msg->tx_bytes[this->byte_counter] = ascii_to_hex(character) << 4;
                this->byte_buffer_index++;
                UARTprintf("%c", character);

            } else {

                this->one_wire_cmd_msg->tx_bytes[this->byte_counter] = this->one_wire_cmd_msg->tx_bytes[this->byte_counter] | ascii_to_hex(character);
                this->byte_buffer_index = 0;
                this->byte_counter++;
                UARTprintf("%c", character);
                if(this->byte_counter < this->one_wire_cmd_msg->num_tx_bytes) {
                    UARTprintf("\nbyte %d : 0x", this->byte_counter + 1);
                }
            }

            if (this->byte_counter >= this->one_wire_cmd_msg->num_tx_bytes ) {
                this->byte_counter = 0;
                this->one_wire_cmd_state = ENTER_NUM_RX_BYTES;
                this->byte_buffer_index = 0;
                UARTprintf("\nEnter number of rx bytes: ");
            }
        }

        break;
    case ENTER_NUM_RX_BYTES :

        if ((character >= '0' && character <= '9') && (this->byte_buffer_index < 3)){

            this->byte_buffer[this->byte_buffer_index] = (uint8_t)character;
            this->byte_buffer_index++;
            UARTprintf("%c", (uint8_t)character);
        } else if ('\r' == character) {
            UARTprintf("%c", (uint8_t)character);
            this->byte_buffer[this->byte_buffer_index] = '\0';

            if(atoi((const char*)this->byte_buffer) > NUM_OF_RX_MSGS) {
                this->byte_buffer_index = 0;
                UARTprintf("\r\nError: Maximum of %d bytes\r\n", NUM_OF_RX_MSGS);
                UARTprintf("Enter number of RX bytes : ");
                break;
            }
            UARTprintf("\r\nPress Space-bar to send message");
            this->one_wire_cmd_msg->num_rx_bytes = atoi((const char*)this->byte_buffer);
            this->byte_buffer_index = 0;
            this->one_wire_cmd_state = ENTER_MESSAGE;
        } else if (character >= '0' && character <= '9') {
            this->byte_buffer_index = 0;
            UARTprintf("\r\nError: Maximum of 2 digits\r\n");
            UARTprintf("Enter number of TX bytes : ");
        }

        break;
    case ENTER_MESSAGE :

        if (character == ' ') {
            this->add_msg(one_wire_cmd_msg);
            this->one_wire_cmd_state = ENTER_NUM_TX_BYTES;
        }

        break;
    default :
        assert(0);
        break;

    }

}

void OneWireCmd::draw_reset(void) {

    this->one_wire_cmd_state = ENTER_NUM_TX_BYTES;
    this->byte_buffer_index = 0;
    this->byte_counter = 0;
    UARTprintf("\r\nEnter number of TX bytes : ");

}

void OneWireCmd::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}


