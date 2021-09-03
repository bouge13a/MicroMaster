/*
 * one_wire_search.cpp
 *
 *  Created on: Aug 30, 2021
 *      Author: steph
 */

#include <assert.h>
#include "one_wire_search.hpp"

#include "uartstdio.h"
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

static const uint32_t RESET_PULSE_TIME_US = 480;
static const uint32_t IDLE_TIME_US = 65;
static const uint32_t AFTER_RESET_WAIT_US = 480;
static const uint32_t START_BIT_TIME_US = 100;
static const uint32_t RELEASE_TIME_US = 65;
static const uint32_t INTER_BIT_TIME_US = 5;
static const uint32_t INTER_BYTE_TIME_US = 500;

static const uint8_t SEARCH_ROM_CMD = 0xF0;
static const uint8_t ALARM_SEARCH_CMD = 0xEC;

static const uint32_t SIZE_OF_ROM_ID = 64;

static void timer2_int_handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntDisable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

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

OneWireSearch::OneWireSearch(GpoObj* gpo_obj,
                             OneWireCmd* one_wire_cmd) : ConsolePage("1 Wire Search",
                                                            portMAX_DELAY,
                                                            false) {
    this->gpo_obj = gpo_obj;
    this->one_wire_cmd = one_wire_cmd;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "1 Wire Search",      /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    timer_semphr = xSemaphoreCreateBinary();

    this->one_wire_q = xQueueCreate(2, sizeof(one_wire_search_types_e));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3));

    TimerClockSourceSet(TIMER3_BASE, TIMER_CLOCK_SYSTEM);
    TimerDisable(TIMER3_BASE, TIMER_A);
    TimerConfigure(TIMER3_BASE, TIMER_CFG_ONE_SHOT);

    IntEnable(INT_TIMER3A);

    TimerIntRegister(TIMER3_BASE, TIMER_A, timer2_int_handler);
    IntPrioritySet(INT_TIMER3A, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    this->one_wire_pin = this->gpo_obj->get_config("GPO 3");

    this->ow_search_state = OW_SEARCH_IDLE;
    this->one_wire_write_state = ONE_WIRE_START;

    this->bit_counter = 0;
    this->rom_id_idx = 0;

    this->error_flag = false;



} // End TestTask

void OneWireSearch::taskfunwrapper(void* parm){
    (static_cast<OneWireSearch*>(parm))->task((OneWireSearch*)parm);
} // End OneWireSearch::taskfunwrapper



void OneWireSearch::task(OneWireSearch* this_ptr) {

    while(1){

        switch(this_ptr->ow_search_state) {
        case OW_SEARCH_IDLE :

            this->error_flag = false;

            xQueueReceive(this_ptr->one_wire_q, &this_ptr->search_type, portMAX_DELAY);

            if( 0 == this_ptr->gpo_obj->get(this_ptr->one_wire_pin)) {
                this_ptr->one_wire_cmd->logger->set_error(this_ptr->one_wire_cmd->pullup_err);
                this_ptr->ow_search_state = OW_SEARCH_FINISH;
                this->error_flag = true;
                break;
            }

            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
            this_ptr->set_timer(RESET_PULSE_TIME_US);

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
            this_ptr->set_timer(IDLE_TIME_US);

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if(this_ptr->gpo_obj->get(this_ptr->one_wire_pin)) {

                this_ptr->one_wire_cmd->logger->set_error(this_ptr->one_wire_cmd->no_resp_err);
                this_ptr->ow_search_state = OW_SEARCH_FINISH;
                this->error_flag = true;

            } else {

                this_ptr->ow_search_state = OW_SEARCH_SEND_CMD;
                this_ptr->set_timer(AFTER_RESET_WAIT_US);

            }


            break;

        case OW_SEARCH_SEND_CMD :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->bit_counter < 8) {

                if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                    SysCtlDelay(START_BIT_TIME_US);

                    this_ptr->one_wire_write_state = ONE_WIRE_STOP;

                    if (this_ptr->search_type == ONE_WIRE_SEARCH_ROM) {
                        if(1 == (SEARCH_ROM_CMD >> (this_ptr->bit_counter)) & 0x0001) {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                        } else {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                        }
                    } else if (this_ptr->search_type == ONE_WIRE_ALARM_SEARCH) {
                        if(1 == (ALARM_SEARCH_CMD >> (this_ptr->bit_counter)) & 0x0001) {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                        } else {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                        }
                    }

                    this_ptr->set_timer(RELEASE_TIME_US);

                } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                    this_ptr->bit_counter++;
                    this_ptr->one_wire_write_state = ONE_WIRE_START;
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                    this_ptr->set_timer(INTER_BIT_TIME_US);

                }
            } else {
                this_ptr->bit_counter = 0;
                this_ptr->ow_search_state = OW_SEARCH_READ_BIT;
                this_ptr->set_timer(INTER_BYTE_TIME_US);
            }


            break;

        case OW_SEARCH_READ_BIT :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                SysCtlDelay(START_BIT_TIME_US);

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

                SysCtlDelay(10);

                this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                this_ptr->bit_id = this_ptr->gpo_obj->get(this_ptr->one_wire_pin);
                this_ptr->set_timer(INTER_BIT_TIME_US);


            } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                this_ptr->one_wire_write_state = ONE_WIRE_START;

                this_ptr->ow_search_state = OW_SEARCH_READ_COMP;

                this_ptr->set_timer(INTER_BIT_TIME_US);

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

            }

            break;

        case OW_SEARCH_READ_COMP :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                SysCtlDelay(START_BIT_TIME_US);

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

                SysCtlDelay(10);

                this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                this_ptr->cmp_bit_id = this_ptr->gpo_obj->get(this_ptr->one_wire_pin);
                this_ptr->set_timer(INTER_BIT_TIME_US);

            } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                this_ptr->one_wire_write_state = ONE_WIRE_START;

                this_ptr->ow_search_state = OW_SEARCH_WRITE_DIR;


                // if bit_id and cmp bit id = 1, no devices participate in search
                if (this_ptr->bit_id && this_ptr->cmp_bit_id) {

                    this_ptr->ow_search_state = OW_SEARCH_FINISH;

                // if bit id and cmp bit id = 0, there are both zeros and ones
                } else if (0 == this_ptr->bit_id && 0 == this_ptr->cmp_bit_id) {

                    if(this_ptr->id_bit_number == this_ptr->last_descrepancy) {

                        this_ptr->search_direction = 1;

                    } else if (this_ptr->id_bit_number > this_ptr->last_descrepancy) {

                        this_ptr->search_direction = 0;

                    } else {

                        this_ptr->search_direction = (this_ptr->rom_ids[this_ptr->rom_id_idx - 1] >> this_ptr->id_bit_number) & 1;

                    }

                    if(this_ptr->search_direction == 0) {
                        this_ptr->last_zero = this_ptr->id_bit_number;

                        if (this_ptr->last_zero < 9) {
                            this_ptr->last_family_discrepancy = this_ptr->last_zero;
                        }
                    }

                } else {
                    this_ptr->search_direction = this_ptr->bit_id;
                }

                this_ptr->rom_ids[this_ptr->rom_id_idx] |= this_ptr->search_direction << this_ptr->id_bit_number;

                this_ptr->id_bit_number++;

                this_ptr->ow_search_state = OW_SEARCH_WRITE_DIR;

                this_ptr->set_timer(INTER_BIT_TIME_US);

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);

            }


            break;

        case OW_SEARCH_WRITE_DIR :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if (this_ptr->one_wire_write_state == ONE_WIRE_START) {

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);

                SysCtlDelay(START_BIT_TIME_US);

                this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                if(1 == this_ptr->search_direction) {
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                } else {
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                }
                this_ptr->set_timer(RELEASE_TIME_US);

            } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {


                if(this_ptr->id_bit_number < SIZE_OF_ROM_ID) {
                    this_ptr->ow_search_state = OW_SEARCH_READ_BIT;
                } else {
                    this_ptr->ow_search_state = OW_SEARCH_FINISH;
                    this_ptr->last_descrepancy = this_ptr->last_zero;
                    if(0 == this_ptr->last_descrepancy) {
                        this_ptr->last_device_flag = true;
                    }
                }

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                this_ptr->set_timer(INTER_BIT_TIME_US);

            }


            break;

        case OW_SEARCH_FINISH :

            this_ptr->last_descrepancy = 0;
            this_ptr->last_family_discrepancy = 0;
            this_ptr->last_device_flag = 0;

            if(!this_ptr->last_device_flag) {
                this_ptr->rom_id_idx++;
                this_ptr->ow_search_state = OW_SEARCH_IDLE;
                xQueueSend(this_ptr->one_wire_q, &this_ptr->search_type, 0);
            }

            UARTprintf("%d", this_ptr->rom_ids[this_ptr->rom_id_idx]);

            this_ptr->ow_search_state = OW_SEARCH_IDLE;
            break;

        default :
            assert(0);
            break;

        }


        vTaskDelay(0);
    }

} // End AdcTask::task

void OneWireSearch::set_timer(uint32_t useconds) {

    TimerDisable(TIMER3_BASE, TIMER_A);
    TimerLoadSet(TIMER3_BASE, TIMER_A, useconds*48);
    TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER3_BASE, TIMER_A);

} // End OneWireCmd::set_timer

void OneWireSearch::draw_page(void) {

}
void OneWireSearch::draw_data(void) {

}
void OneWireSearch::draw_input(int character) {

    if ('s' == character) {
        xQueueSend(this->one_wire_q, &SEARCH_ROM_CMD, 0);
    } else if ('a' == character) {
        xQueueSend(this->one_wire_q, &ALARM_SEARCH_CMD, 0);
    }

}

void OneWireSearch::draw_reset(void) {

}

void OneWireSearch::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
