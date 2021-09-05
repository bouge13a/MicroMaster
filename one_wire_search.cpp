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
static const uint32_t START_BIT_TIME_US = 65;
static const uint32_t RELEASE_TIME_US = 40;
static const uint32_t INTER_BIT_TIME_US = 5;
static const uint32_t INTER_BYTE_TIME_US = 500;

static const uint8_t SEARCH_ROM_CMD = 0xF0;
static const uint8_t ALARM_SEARCH_CMD = 0xEC;

static const uint32_t SIZE_OF_ROM_ID = 64;

static unsigned char crc8;

static unsigned char dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};


unsigned char OneWireSearch::crc_check(unsigned char value) {

    crc8 = dscrc_table[crc8 ^ value];
    return crc8;
}

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

    this->one_wire_pin = this->gpo_obj->get_config("one wire");

    this->ow_search_state = OW_SEARCH_IDLE;
    this->one_wire_write_state = ONE_WIRE_START;

    this->bit_counter = 0;
    this->rom_id_idx = 0;

    this->error_flag = false;

    this->id_bit_number = 1;



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

                    if (this_ptr->search_type == SEARCH_ROM_CMD) {
                        if(1 == ((SEARCH_ROM_CMD >> this_ptr->bit_counter) & 0x0001)) {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                        } else {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                        }
                    } else if (this_ptr->search_type == ALARM_SEARCH_CMD) {
                        if(1 == ((ALARM_SEARCH_CMD >> this_ptr->bit_counter) & 0x0001)) {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                        } else {
                            this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 0);
                        }
                    }

                    this_ptr->set_timer(RELEASE_TIME_US);

                } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                    this_ptr->bit_counter++;
                    this_ptr->set_timer(INTER_BIT_TIME_US);
                    this_ptr->one_wire_write_state = ONE_WIRE_START;
                    this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);


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

                //SysCtlDelay(5);

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

                this_ptr->one_wire_write_state = ONE_WIRE_STOP;
                this_ptr->cmp_bit_id = this_ptr->gpo_obj->get(this_ptr->one_wire_pin);
                this_ptr->set_timer(INTER_BIT_TIME_US);

            } else if (this_ptr->one_wire_write_state == ONE_WIRE_STOP) {

                this_ptr->one_wire_write_state = ONE_WIRE_START;

                this_ptr->ow_search_state = OW_SEARCH_WRITE_DIR;

                this_ptr->set_timer(INTER_BIT_TIME_US);

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);


                // if bit_id and cmp bit id = 1, no devices participate in search

                if (this_ptr->bit_id && this_ptr->cmp_bit_id) {

                    this_ptr->ow_search_state = OW_SEARCH_FINISH;
                    this_ptr->last_device_flag = true;
                    break;

                // if bit id and cmp bit id = 0, there are both zeros and ones
                } else if ((0 == this_ptr->bit_id) && (0 == this_ptr->cmp_bit_id)) {

                    if(this_ptr->id_bit_number < this_ptr->last_descrepancy) {

                        this_ptr->search_direction = (this_ptr->rom_ids[this_ptr->rom_id_idx] >> (this_ptr->id_bit_number-1)) & 1;

                    } else {

                        this_ptr->search_direction = (this_ptr->id_bit_number == this_ptr->last_descrepancy);
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

                if (this_ptr->search_direction == 1) {
                    this_ptr->rom_ids[this_ptr->rom_id_idx] |= this_ptr->search_direction << (this_ptr->id_bit_number-1);
                } else {
                    this_ptr->rom_ids[this_ptr->rom_id_idx] &= ~(this_ptr->search_direction << (this_ptr->id_bit_number-1));
                }

                this_ptr->id_bit_number++;

                this_ptr->ow_search_state = OW_SEARCH_WRITE_DIR;



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

                this_ptr->one_wire_write_state = ONE_WIRE_START;

                this_ptr->gpo_obj->set(this_ptr->one_wire_pin, 1);
                this_ptr->set_timer(INTER_BIT_TIME_US);

                if(this_ptr->id_bit_number <= SIZE_OF_ROM_ID) {
                    this_ptr->ow_search_state = OW_SEARCH_READ_BIT;
                    break;
                }

                for(uint32_t index=0; index<8; index++) {
                    crc_check((uint8_t)((this_ptr->rom_ids[this_ptr->rom_id_idx] >> (index*8)) & 0xff));
                }

                if (crc8 == 0) {

                    this_ptr->id_bit_number = 1;
                    this_ptr->bit_counter = 0;
                    this_ptr->ow_search_state = OW_SEARCH_FINISH;
                    this_ptr->last_descrepancy = this_ptr->last_zero;
                    if(0 == this_ptr->last_descrepancy) {
                        this_ptr->last_device_flag = true;
                    }
                    this->search_result = true;
                } else {
                    this_ptr->id_bit_number = 1;
                    this_ptr->bit_counter = 0;
                    this_ptr->ow_search_state = OW_SEARCH_FINISH;
                    this->search_result = false;
                }
            }


            break;

        case OW_SEARCH_FINISH :

            xSemaphoreTake( timer_semphr, portMAX_DELAY);

            if(!this_ptr->search_result) {
                this_ptr->last_descrepancy = 0;
                this_ptr->last_family_discrepancy = 0;
                this_ptr->last_device_flag = false;
                this_ptr->ow_search_state = OW_SEARCH_IDLE;
                UARTprintf("\r\nSearch Ended");
                break;
            }

            if(!this_ptr->last_device_flag) {
                this_ptr->rom_id_idx++;
                this_ptr->ow_search_state = OW_SEARCH_IDLE;
                xQueueSend(this_ptr->one_wire_q, &this_ptr->search_type, 0);
                break;
            }

            this_ptr->last_descrepancy = 0;
            this_ptr->last_family_discrepancy = 0;

            for (uint32_t index=0; index<=this_ptr->rom_id_idx; index++) {
                for(uint32_t inner_index=0; inner_index<8; inner_index++) {
                    UARTprintf("0x%02x ", (uint8_t)(this_ptr->rom_ids[index] >> (inner_index*8)) & 0xff);
                }
            }

            if(this_ptr->last_device_flag) {
                this_ptr->rom_id_idx = 0;
            }
            this_ptr->last_device_flag = false;

            UARTprintf("\r\n");

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

    this->last_descrepancy = 0;
    this->last_family_discrepancy = 0;
    this->last_device_flag = false;
    this->rom_id_idx = 1;
}

void OneWireSearch::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}