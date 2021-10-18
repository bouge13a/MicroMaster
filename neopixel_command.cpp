/*
 * neopixel_control.cpp
 *
 *  Created on: Oct 8, 2021
 *      Author: steph
 */

#include <console_uart.hpp>
#include <neopixel_command.hpp>
#include <neopixel_command.hpp>
#include "uartstdio.h"
#include "text_controls.hpp"

#include "driverlib/inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"

#include "utils.hpp"

static const uint32_t NUM_OF_BITS = 24;
static const uint32_t SIZE_OF_QUEUE = 100;

static const uint32_t NUM_TX_MSGS = 99;
static const uint32_t MAX_TX_DIGITS = 2;

static const uint32_t RESET_DELAY_US = 50;

static SemaphoreHandle_t timer_semphr = NULL;

static void timer0_int_handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_AHB_BASE,
                     GPIO_PIN_1,
                     0);

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

NeopixMsg::NeopixMsg(neopix_msg_types_e msg_type) {

    this->msg_type = msg_type;
    this->msgs_txed=0;
    this->msg_state = neopix_idle;

} // End NeopixMsg::NeopixMsg

void NeopixelCtl::taskfunwrapper(void* parm){
    (static_cast<NeopixelCtl*>(parm))->task((NeopixelCtl*)parm);
} // End NeopixelCtl::taskfunwrapper


NeopixelCtl::NeopixelCtl(void) : ConsolePage("NeoPixel Command",
                                                     portMAX_DELAY,
                                                     false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Neopixel Ctl",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->neopix_msg_q = xQueueCreate(SIZE_OF_QUEUE, sizeof(NeopixMsg*));
    timer_semphr = xSemaphoreCreateBinary();

    this->bit_counter = 0;
    this->byte_buffer_index = 0;
    this->msg_counter = 0;

    this->cmd_state = NEOPIX_CMD_GET_NUM_TX_MSGS;
    this->neopix_state = NEOPIX_IDLE;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    IntEnable(INT_TIMER0A);

    TimerIntRegister(TIMER0_BASE, TIMER_A, timer0_int_handler);
    IntPrioritySet(INT_TIMER0A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    this->byte_buffer = new uint8_t[MAX_TX_DIGITS];

    this->neopix_cmd_msg = new NeopixMsg(neopix_command_msg);
    this->neopix_cmd_msg->tx_msgs = new uint32_t[NUM_TX_MSGS];

} // End NeopixelCtl


void NeopixelCtl::task(NeopixelCtl* this_ptr) {

    while(1){

        switch(this_ptr->neopix_state) {
        case NEOPIX_IDLE :

            xQueueReceive(this_ptr->neopix_msg_q, &this_ptr->neopix_msg, portMAX_DELAY);

            assert(this_ptr->neopix_msg);

            this_ptr->neopix_msg->msg_state = neopix_processing;
            this_ptr->neopix_msg->msgs_txed = 0;

            this_ptr->deinit_spi();
            this_ptr->init_gpo();

            //this_ptr->set_timer(50);
            //this_ptr->set_gpo(1);

            //1xSemaphoreTake( timer_semphr, portMAX_DELAY);

            //this_ptr->set_gpo(0);

            this_ptr->deinit_gpo();

            this_ptr->init_spi();

            this_ptr->neopix_state = NEOPIX_SEND;

            break;

        case NEOPIX_SEND :

            while(SSIBusy(SSI1_BASE)) {
                break;
            }

            if(this_ptr->bit_counter < NUM_OF_BITS) {

                this_ptr->send_bit((this_ptr->neopix_msg->tx_msgs[this_ptr->neopix_msg->msgs_txed] >> this_ptr->bit_counter) & 0x00000001);

                this_ptr->bit_counter++;

            } else {

                this_ptr->bit_counter = 0;
                this_ptr->neopix_msg->msgs_txed++;

                if (this_ptr->neopix_msg->msgs_txed >= this_ptr->neopix_msg->num_tx_msgs) {
                    this_ptr->neopix_state = NEOPIX_FINISH;
                }
            }

            break;

        case NEOPIX_FINISH :

            this->neopix_state = NEOPIX_IDLE;
            vTaskDelay(0);

            break;

        default :
            assert(0);
            break;
        }



    }

} // End AdcTask::task

void NeopixelCtl::add_msg(NeopixMsg* msg) {

    xQueueSend(this->neopix_msg_q, &msg, portMAX_DELAY);

}

void NeopixelCtl::init_spi(void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI1));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOPinConfigure(GPIO_PF0_SSI1RX);
    GPIOPinConfigure(GPIO_PF1_SSI1TX);
    GPIOPinConfigure(GPIO_PF2_SSI1CLK);
    GPIOPinConfigure(GPIO_PF3_SSI1FSS);

    GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    SSIConfigSetExpClk(SSI1_BASE,
                       SysCtlClockGet(),
                       SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER,
                       4800000,
                       8);

    SSIEnable(SSI1_BASE);

} // End NeopixelCtl::init_spi

void NeopixelCtl::deinit_spi(void) {

    //SSIDisable(SSI1_BASE);
    SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI1);

} // End NeopixelCtl::deinit_spi

void NeopixelCtl::init_gpo(void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF);

    GPIODirModeSet(GPIO_PORTF_AHB_BASE,
                       GPIO_PIN_1,
                       GPIO_DIR_MODE_OUT);

    GPIOPadConfigSet(GPIO_PORTF_AHB_BASE,
                         GPIO_PIN_1,
                         GPIO_STRENGTH_12MA,
                         GPIO_PIN_TYPE_STD_WPD);

} // End NeopixelCtl::init_gpo

void NeopixelCtl::deinit_gpo(void) {

    SysCtlGPIOAHBDisable(SYSCTL_PERIPH_GPIOF);

} // End NeopixelCtl::deinit_gpo

void NeopixelCtl::set_gpo(uint32_t value) {

    if(value > 0) {
        GPIOPinWrite(GPIO_PORTF_AHB_BASE,
                         GPIO_PIN_1,
                         GPIO_PIN_1);
    } else {
        GPIOPinWrite(GPIO_PORTF_AHB_BASE,
                         GPIO_PIN_1,
                         0);
    }

} // End NeopixelCtl::set_gpo

void NeopixelCtl::set_timer(uint32_t useconds) {

    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerLoadSet(TIMER0_BASE, TIMER_A, useconds*80);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

} // End OneWireCmd::set_timer

void NeopixelCtl::send_bit(uint32_t bit) {

    if (bit) {
        SSIDataPut(SSI1_BASE, 0x7f);
    } else {
        SSIDataPut(SSI1_BASE, 0x1f);
    }

} // End NeopixelCtl::send_byte

void NeopixelCtl::draw_page(void) {

    UARTprintf("Enter number of NeoPixels: ");

}
void NeopixelCtl::draw_data(void) {

}
void NeopixelCtl::draw_input(int character) {


    switch(this->cmd_state) {
    case NEOPIX_CMD_GET_NUM_TX_MSGS :

        if ((character >= '0' && character <= '9') && (this->byte_buffer_index < NUM_TX_MSGS)) {

            this->byte_buffer[this->byte_buffer_index] = (uint8_t)character;
            this->byte_buffer_index++;
            UARTprintf("%c", (uint8_t)character);
        } else if ('\r' == character) {
            UARTprintf("%c", (uint8_t)character);
            this->byte_buffer[this->byte_buffer_index] = '\0';

            if(atoi((const char*)this->byte_buffer) > NUM_TX_MSGS) {
                this->byte_buffer_index = 0;
                UARTprintf("\r\nError: Maximum of %d messages\r\n", NUM_TX_MSGS);
                UARTprintf("Enter number of NeoPixels : ");
                break;
            }

            this->neopix_cmd_msg->num_tx_msgs = atoi((const char*)this->byte_buffer);
            this->cmd_state = NEOPIX_CMD_GET_TX_MSGS;
            UARTprintf("\r\nNeopixel 1 0x");
            this->byte_buffer_index = 0;

        } else if(character >= '0' && character <= '9'){
            this->byte_buffer_index = 0;
            UARTprintf("\r\nError: Maximum of 99 Neopixels\r\n");
            UARTprintf("Enter number of NeoPixels: ");

        }  else if (character == 127) {
            if(this->byte_buffer_index > 0) {
                this->byte_buffer_index--;
                UARTprintf("\b");
                TextCtl::clear_in_line();
                return;
            }

        } else {
            this->send_bell();
        }

        break;

    case NEOPIX_CMD_GET_TX_MSGS :

        if ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f')){

            if (this->msg_counter < this->neopix_cmd_msg->num_tx_msgs) {
                if (this->byte_buffer_index <= 6) {

                    UARTprintf("%c", character);
                    this->neopix_cmd_msg->tx_msgs[this->msg_counter] |= ascii_to_hex(character) << (this->byte_buffer_index*4);
                    this->byte_buffer_index++;

                    if(this->byte_buffer_index == 6) {
                        this->byte_buffer_index = 0;
                        this->msg_counter++;

                        if (this->msg_counter == this->neopix_cmd_msg->num_tx_msgs) {

                            this->cmd_state = NEOPIX_SEND_MSG;
                            UARTprintf("\r\nPress spacebar to send message\r\n");
                            this->msg_counter = 0;
                            break;
                        }

                        UARTprintf("\r\nNeopixel %d 0x", this->msg_counter + 1);
                    }
                } else {
                    assert(0);
                }

            }
        } else {
            this->send_bell();
        }

        break;
    case NEOPIX_SEND_MSG:

        if (character == ' ') {

            this->add_msg(this->neopix_cmd_msg);
            UARTprintf("\r\nEnter number of NeoPixels: ");
            this->cmd_state = NEOPIX_CMD_GET_NUM_TX_MSGS;

        }  else {
            this->send_bell();
        }

        break;

    default :
        assert(0);
        break;
    }

}

void NeopixelCtl::draw_reset(void) {

    this->byte_buffer_index = 0;
    this->msg_counter = 0;
    this->bit_counter = 0;
    this->cmd_state = NEOPIX_CMD_GET_NUM_TX_MSGS;
    UARTprintf("\r\nEnter number of NeoPixels: ");

}

void NeopixelCtl::draw_help(void) {


}
