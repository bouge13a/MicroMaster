/*
 * UART_command.cpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */


#include "UART_command.hpp"
#include "uartstdio.h"
#include "text_controls.hpp"

#include "rom.h"
#include "rom_map.h"
#include "sysctl.h"
#include "gpio.h"
#include "uart.h"
#include "pin_map.h"
#include "interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

static const uint32_t SIZE_OF_CHAR_STRING = 50;
static const uint32_t MAX_FREQUENCY = 115200;
static const uint32_t MIN_FREQUENCY = 300;

static uint32_t uart_speed = 115200;
static uint32_t uart_parity = UART_CONFIG_PAR_NONE;
static uint32_t uart_stop_bits = UART_CONFIG_STOP_ONE;

void uart_set_parity(uint32_t index) {

    switch(index) {
    case 0 :
        uart_parity = UART_CONFIG_PAR_NONE;
        break;
    case 1 :
        uart_parity = UART_CONFIG_PAR_EVEN;
        break;
    case 2 :
        uart_parity = UART_CONFIG_PAR_ODD;
        break;
    case 3 :
        uart_parity = UART_CONFIG_PAR_ONE;
        break;
    case 4 :
        uart_parity = UART_CONFIG_PAR_ZERO;
        break;
    default :
        assert(0);
        break;
    }

    UARTDisable(UART1_BASE);
    UARTParityModeSet(UART1_BASE, uart_parity);
    UARTEnable(UART1_BASE);

} // End uart_set_parity

void uart_set_stop_bits(uint32_t index) {

    switch (index) {
    case 0 :
        uart_stop_bits = UART_CONFIG_STOP_ONE;
        break;
    case 1 :
        uart_stop_bits = UART_CONFIG_STOP_TWO;
        break;
    default :
        assert(0);
    }


    UARTDisable(UART1_BASE);
    UARTConfigSetExpClk(UART1_BASE,
                        MAP_SysCtlClockGet(),
                        uart_speed,
                        (UART_CONFIG_WLEN_8 | uart_stop_bits | uart_parity));
    UARTEnable(UART1_BASE);

} // End uart_set_stop_bits


UartCmd::UartCmd(void)  : ConsolePage("UART Command",
                                      portMAX_DELAY,
                                      true){


    this->char_string = new char[SIZE_OF_CHAR_STRING];
    this->char_string_index = 0;
    this->last_char_string_index = 0;
    this->cmd_state = UART_GET_SPEED;

    this->msg_type = UART_GET_STRING;

} // End UartCmdTask::UartCmdTask

static uint32_t ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
}

void UartCmd::send_message(void) {

    if (this->last_char_string_index != 0) {
        UARTprintf("\r\n");
    }

    if (this->msg_type == UART_GET_STRING) {

        for (uint32_t index=0; index<this->last_char_string_index; index++) {

            UARTCharPut(UART1_BASE, this->char_string[index] );

            if(0 == index) {
                UARTprintf("TX: %s", this->char_string);
            }
        }
    } else if (this->msg_type == UART_GET_HEX) {

        for (uint32_t index=0; index<this->last_char_string_index; index += 2) {

            UARTCharPut(UART1_BASE, ascii_to_hex(this->char_string[index+1]) | (ascii_to_hex(this->char_string[index]) << 4) );

            if(0 == index) {
                UARTprintf("TX : 0x%x", ascii_to_hex(this->char_string[index+1]) | ascii_to_hex(this->char_string[index]) << 4 );
            } else {
                UARTprintf("%x", ascii_to_hex(this->char_string[index+1]) | ascii_to_hex(this->char_string[index]) << 4 );
            }
        }
    }

    if (this->last_char_string_index != 0) {
        UARTprintf("\r\n");
    }
} // End UartCmd::send_message

void UartCmd::draw_page(void) {

    UARTprintf("The help feature is disabled for this page\r\n");
    UARTprintf("Enter speed (300 - 115200 Hz) : ");

}

void UartCmd::draw_data(void) {

}

void UartCmd::draw_input(int character) {

    switch(this->cmd_state) {
    case UART_GET_SPEED :

        if ((character >= '0') && (character <= '9')) {
            this->char_string[this->char_string_index] = (char)character;
            this->char_string_index++;
            UARTprintf("%c", (char)character);
        } else if (character == '\r') {

            this->char_string[this->char_string_index] = '\0';

            if(atoi((const char*)this->char_string) > MAX_FREQUENCY || atoi((const char*)this->char_string) < MIN_FREQUENCY) {
                this->char_string_index = 0;
                UARTprintf("\r\nError: Speed out of range\r\n");
                UARTprintf("Enter speed (300 - 115200 bps) : ");
                break;
            }

            uart_speed = atoi((const char*)this->char_string);

            UARTDisable(UART1_BASE);
            UARTConfigSetExpClk(UART1_BASE,
                                MAP_SysCtlClockGet(),
                                uart_speed,
                                (UART_CONFIG_WLEN_8 | uart_stop_bits | uart_parity));
            UARTEnable(UART1_BASE);

            this->char_string_index = 0;
            this->cmd_state = UART_GET_FORMAT;
            UARTprintf("\r\nEnter format (t for text, h for hex) : ");
        }

        break;

    case UART_GET_FORMAT :

        if ('t' == character) {
            this->cmd_state = UART_GET_STRING;
            UARTprintf("t\r\nEnter string : ");
            this->msg_type = UART_GET_STRING;
        } else if ('h' == character) {
            this->cmd_state = UART_GET_HEX;
            UARTprintf("h\r\nEnter hex : 0x");
            this->msg_type = UART_GET_HEX;
        }

        break;

    case UART_GET_STRING :

        if(character != '\r') {

            this->char_string[this->char_string_index] = (char)character;
            this->char_string_index++;
            UARTprintf("%c", (char)character);

            if (this->char_string_index > SIZE_OF_CHAR_STRING) {
                this->char_string_index = 0;
                UARTprintf("\r\nError: too many characters");
                UARTprintf("\r\nEnter string : ");
                break;
            }

        } else {

            for(uint32_t index=0; index<this->char_string_index; index++) {
                UARTCharPut(UART1_BASE, this->char_string[index]);
            }

            this->cmd_state = UART_GET_SPEED;
            this->last_char_string_index = this->char_string_index;
            this->char_string_index = 0;
            UARTprintf("\r\nMessage transmitted");
            UARTprintf("\r\nEnter speed (300 - 115200 Hz) : ");

        }

        break;

    case UART_GET_HEX :
        if((character >= '0' && character <='9') || (character >='a' && character <= 'f')) {

            this->char_string[this->char_string_index] = (char)character;
            this->char_string_index++;
            UARTprintf("%c", (char)character);

            if (this->char_string_index > SIZE_OF_CHAR_STRING) {
                this->char_string_index = 0;
                UARTprintf("\r\nError: too many characters");
                UARTprintf("\r\nEnter string : ");
                break;
            }

        } else if (character == '\r') {

            for (uint32_t index=0; index<this->char_string_index; index += 2) {
                UARTCharPut(UART1_BASE, ascii_to_hex(this->char_string[index+1]) | ascii_to_hex(this->char_string[index]) << 4 );
            }

            this->cmd_state = UART_GET_SPEED;
            this->last_char_string_index = this->char_string_index;
            this->char_string_index = 0;
            UARTprintf("\r\nMessage transmitted");
            UARTprintf("\r\nEnter speed (300 - 115200 Hz) : ");

        }
        break;

    default :
        assert(0);
        break;
    }
}

void UartCmd::draw_reset(void) {

    this->char_string_index = 0;
    this->cmd_state = UART_GET_SPEED;
    UARTprintf("\r\nEnter speed (300 - 115200 Hz) : ");
}

void UartCmd::draw_help(void) {

}
