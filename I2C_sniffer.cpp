/*
 * I2C_sniffer.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: steph
 */




/*
 * I2C_sniffer.cpp
 *
 *  Created on: Sep 7, 2021
 *      Author: steph
 */


#include <I2C_sniffer.hpp>
#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_ints.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "uartstdio.h"

static GpiObj* gpo_object;

static gpio_pin_t* scl_pin;
static gpio_pin_t* data_pin;

static volatile uint32_t bit_counter = 0;

static volatile i2c_sniff_states_e i2c_sniff_state = I2CS_STOP_CONDITION;

static volatile uint32_t start_bit = 0;

static QueueHandle_t i2c_sniff_q = NULL;

static uint32_t byte_buffer = 0;

static const uint32_t I2CS_START_CHAR = 256;
static const uint32_t I2CS_STOP_CHAR = 257;
static const uint32_t I2CS_ACK_CHAR = 258;
static const uint32_t I2CS_NACK_CHAR = 259;

static uint32_t status_d = 0;
static uint32_t status_f = 0;

static void scl_int_handler(void) {

    status_f = GPIOIntStatus(scl_pin->port, false);
    GPIOIntClear(scl_pin->port, scl_pin->pin);

    if (scl_pin->pin != status_f & scl_pin->pin) {
        return;
    }

    switch(i2c_sniff_state) {
    case I2CS_STOP_CONDITION :

        i2c_sniff_state = I2CS_GET_DATA;

        break;

    case I2CS_GET_DATA :

        if (bit_counter < 8) {

            if(gpo_object->get(data_pin)) {
                byte_buffer |= (1 << (7 - bit_counter));
            } else {
                byte_buffer &= ~(1 << (7- bit_counter));
            }

            bit_counter++;

        } else if (bit_counter == 8) {

            xQueueSendFromISR(i2c_sniff_q, &byte_buffer, 0);

            if(gpo_object->get(data_pin)) {
                xQueueSendFromISR(i2c_sniff_q, &I2CS_NACK_CHAR, 0);
            } else {
                xQueueSendFromISR(i2c_sniff_q, &I2CS_NACK_CHAR, 0);
            }

            bit_counter=0;
            byte_buffer = 0;

            i2c_sniff_state = I2CS_DETERMINE_CONDITION;

            GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_RISING_EDGE);

            GPIOIntEnable(data_pin->port, data_pin->pin);


        }


        break;
    case I2CS_DETERMINE_CONDITION :

        GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_RISING_EDGE);

        start_bit = gpo_object->get(data_pin);

        i2c_sniff_state = I2CS_DETERMINE_STOP;

        break;

    case I2CS_DETERMINE_STOP :

        if(start_bit) {
            byte_buffer |= (1 << 7);
        } else {
            byte_buffer &= ~(1<<7);
        }

        bit_counter++;

        GPIOIntDisable(data_pin->port, data_pin->pin);

        i2c_sniff_state = I2CS_GET_DATA;

        break;
    case I2CS_REPEATED_START :
        break;
    default :
        break;
    }

}


static void data_int_handler(void) {

    status_d = GPIOIntStatus(data_pin->port, false);
    GPIOIntClear(data_pin->port, data_pin->pin);

    if (data_pin->pin != status_d & data_pin->pin ) {
        return;
    }

    switch(i2c_sniff_state) {
    case I2CS_STOP_CONDITION :

        xQueueSendFromISR(i2c_sniff_q, &I2CS_START_CHAR, 0);

        GPIOIntDisable(scl_pin->port, scl_pin->pin);
        GPIOIntTypeSet(scl_pin->port, scl_pin->pin, GPIO_RISING_EDGE);
        GPIOIntEnable(scl_pin->port, scl_pin->pin);

        GPIOIntDisable(data_pin->port, data_pin->pin);

        i2c_sniff_state = I2CS_GET_DATA;

        break;
    case I2CS_GET_DATA :


        break;

    case I2CS_DETERMINE_CONDITION :

        i2c_sniff_state = I2CS_READ_BYTE;

        GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_FALLING_EDGE);

        break;

    case I2CS_READ_BYTE :

        i2c_sniff_state = I2CS_GET_DATA;

        GPIOIntDisable(data_pin->port, data_pin->pin);

        GPIOIntDisable(scl_pin->port, scl_pin->pin);
        GPIOIntTypeSet(scl_pin->port, scl_pin->pin, GPIO_RISING_EDGE);
        GPIOIntEnable(scl_pin->port, scl_pin->pin);
        break;

    case I2CS_REPEATED_START :

        i2c_sniff_state = I2CS_STOP_CONDITION;

        break;

    case I2CS_DETERMINE_STOP :

        xQueueSendFromISR(i2c_sniff_q, &I2CS_STOP_CHAR, 0);

        GPIOIntDisable(scl_pin->port, scl_pin->pin);

        i2c_sniff_state = I2CS_STOP_CONDITION;
        break;
    default :
        break;
    }

}

I2cSniffer::I2cSniffer(GpiObj* gpo_obj) : ConsolePage("I2C Sniffer",
                                                      portMAX_DELAY,
                                                      false) {

    gpo_object = gpo_obj;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C Sniffer",                            /* Text name for the task. */
                80,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL );                      /* Used to pass out the created task's handle. */

    i2c_sniff_q = xQueueCreate(20, sizeof(uint32_t));

    scl_pin = gpo_obj->get_config("GPI 1");
    data_pin = gpo_obj->get_config("GPI 2");

    GPIOIntDisable(scl_pin->port, scl_pin->pin);
    GPIOIntClear(scl_pin->port, scl_pin->pin);
    GPIOIntRegister(scl_pin->port, scl_int_handler);
    GPIOIntTypeSet(scl_pin->port, scl_pin->pin, GPIO_RISING_EDGE);

    GPIOIntDisable(data_pin->port, data_pin->pin);
    GPIOIntClear(data_pin->port, data_pin->pin);
    GPIOIntRegister(data_pin->port, data_int_handler);
    GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_FALLING_EDGE);

    IntPrioritySet(INT_GPIOD, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);
    IntPrioritySet(INT_GPIOF, configMAX_SYSCALL_INTERRUPT_PRIORITY+1);

    IntEnable(INT_GPIOF);
    IntEnable(INT_GPIOD);

} // End I2cSniffer

void I2cSniffer::taskfunwrapper(void* parm){
    (static_cast<I2cSniffer*>(parm))->task((I2cSniffer*)parm);
} // End I2cSniffer::taskfunwrapper

void I2cSniffer::task(I2cSniffer* this_ptr) {

    while(1) {

        xQueueReceive(i2c_sniff_q, &this_ptr->rx_char, portMAX_DELAY);

        switch(rx_char) {
        case I2CS_START_CHAR :
            UARTprintf("[");
            break;
        case I2CS_STOP_CHAR :
            UARTprintf("]\r\n");
            break;
        case I2CS_ACK_CHAR :
            UARTprintf("ACK, ");
            break;
        case I2CS_NACK_CHAR :
            UARTprintf("NACK, ");
            break;
        default :
            UARTprintf("(0x%x)", this_ptr->rx_char);
            break;
        }

    }

}


void I2cSniffer::draw_page(void) {

    GPIOIntEnable(scl_pin->port, scl_pin->pin);
    GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_FALLING_EDGE);
    GPIOIntEnable(data_pin->port, data_pin->pin);


}
void I2cSniffer::draw_data(void) {

}
void I2cSniffer::draw_input(int character) {

}

void I2cSniffer::draw_reset(void) {

    GPIOIntDisable(scl_pin->port, scl_pin->pin);
    GPIOIntDisable(data_pin->port, data_pin->pin);

    GPIOIntTypeSet(data_pin->port, data_pin->pin, GPIO_FALLING_EDGE);
    GPIOIntTypeSet(scl_pin->port, scl_pin->pin, GPIO_FALLING_EDGE);


    i2c_sniff_state = I2CS_STOP_CONDITION;
    bit_counter = 0;

}

void I2cSniffer::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}
