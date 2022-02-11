/*
 * pin_list_page.cpp
 *
 *  Created on: Feb 11, 2022
 *      Author: steph
 */

#include "pin_list_page.hpp"

#include <stdint.h>
#include "uartstdio.h"

#include "text_controls.hpp"

typedef struct {
    const char* pin_num;
    const char* description;
}pin_descr_t;

static const pin_descr_t pin_descriptions[] = {
    {"PA2", "SPI Clock"},
    {"PA3", "SPI Chip Select"},
    {"PA4", "SPI Rx"},
    {"PA5", "SPI Tx"},
    {"PA6", "I2C SCL"},
    {"PA7", "I2C SDA"},
    {"PB2", "I2C Sniffer SCL"},
    {"PB3", "I2C Sniffer SDA"},
    {"PB4", "CAN Rx"},
    {"PB5", "CAN Tx"},
    {"PB6", "PWM"},
    {"PC4", "UART Rx"},
    {"PC5", "UART Tx"},
    {"PD0", "GPI 0"},
    {"PC6", "GPI 1"},
    {"PB7", "GPI 2"},
    {"PD1", "GPI 3"},
    {"PF0", "GPO 0"},
    {"PF1", "GPO 1"},
    {"PF2", "GPO 2"},
    {"PF3", "GPO 3"},
    {"PE3", "ADC 0"},
    {"PE2", "ADC 1"},
    {"PE1", "ADC 2"},
};

static const uint32_t pin_descriptions_size = sizeof(pin_descriptions)/sizeof(pin_descriptions[0]);

static const uint32_t DESCR_COL = 8;
static const uint32_t SECOND_START_COL = 50;
static const uint32_t START_ROW = 6;

PinPage::PinPage(void) : ConsolePage("Pin List",
                                      portMAX_DELAY,
                                      false) {

}

void PinPage::draw_page(void) {

    TextCtl::cursor_pos(START_ROW, 0);
    UARTprintf("Pin");
    TextCtl::cursor_pos(START_ROW, DESCR_COL);
    UARTprintf("Description");
    TextCtl::cursor_pos(START_ROW, SECOND_START_COL);
    UARTprintf("Pin");
    TextCtl::cursor_pos(START_ROW, DESCR_COL + SECOND_START_COL);
    UARTprintf("Description");

    for (uint32_t index=0; index<pin_descriptions_size; index++) {

        if (index < pin_descriptions_size/2) {

            TextCtl::cursor_pos(START_ROW + 2 + index, 0);
            UARTprintf("%s", pin_descriptions[index].pin_num);
            TextCtl::cursor_pos(START_ROW + 2 + index, DESCR_COL);
            UARTprintf("%s", pin_descriptions[index].description);

        } else {

            TextCtl::cursor_pos(START_ROW + 2 + index - pin_descriptions_size/2, SECOND_START_COL);
            UARTprintf("%s", pin_descriptions[index].pin_num);
            TextCtl::cursor_pos(START_ROW + 2 + index - pin_descriptions_size/2, DESCR_COL + SECOND_START_COL);
            UARTprintf("%s", pin_descriptions[index].description);

        }
    }
}
void PinPage::draw_data(void) {

}
void PinPage::draw_input(int character) {

}
void PinPage::draw_help(void) {

}
void PinPage::draw_reset(void) {

}
