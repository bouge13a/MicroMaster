/*
 * io_control_page.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#include "io_control_page.hpp"
#include <uartstdio.h>
#include "text_controls.hpp"
#include "driverlib/interrupt.h"
#include "driverlib/inc/hw_ints.h"

static const uint32_t START_ROW = 6;

static const uint32_t GPO_NAME_COL = 1;
static const uint32_t GPI_NAME_COL = 31;
static const uint32_t ADC_NAME_COL = 61;

static const uint32_t GPO_DATA_COL = 15;
static const uint32_t GPI_DATA_COL = 45;
static const uint32_t ADC_DATA_COL = 75;

static const uint32_t NUM_OF_DISPLAYED_GPOS = 4;


IoCtlPage::IoCtlPage(GpoObj* gpo_obj,
                     GpiObj* gpi_obj,
                     AdcTask* adc_obj)  : ConsolePage("IO Control",
                                                      200,
                                                      false) {

    this->gpo_obj = gpo_obj;
    this->gpi_obj = gpi_obj;
    this->adc_task = adc_obj;

    this->gpo_struct = gpo_obj->get_struct();
    this->gpi_struct = gpi_obj->get_struct();
    this->adc_struct = adc_task->get_adc_struct();

    this->gpo_index = 0;

} // End IoCtlPage::IoCtlPage

void IoCtlPage::draw_page(void) {

    // Enable the interrupt for ADC0 sequence 0 on the processor (NVIC).
    IntEnable(INT_ADC0SS0);

    uint32_t print_index = 0;

    TextCtl::cursor_pos(START_ROW, GPO_NAME_COL);

    UARTprintf("Outputs");

    TextCtl::text_color(TextCtl::yellow_text);

    for(uint32_t index=0; index<gpo_struct->num_gpos; index++) {
        if (gpo_struct->gpos[index]->displayed) {
            TextCtl::cursor_pos(START_ROW + print_index + 1, GPO_NAME_COL);
            print_index++;
            UARTprintf("%s", gpo_struct->gpos[index]->name);
        }
    }

    print_index = 0;

    TextCtl::cursor_pos(START_ROW, GPI_NAME_COL);

    TextCtl::text_color(TextCtl::white_text);
    UARTprintf("Inputs");
    TextCtl::text_color(TextCtl::cyan_text);

    for(uint32_t index=0; index<gpi_struct->num_gpis; index++) {
        if (gpi_struct->gpis[index]->displayed) {
            TextCtl::cursor_pos(START_ROW + print_index + 1, GPI_NAME_COL);
            print_index++;
            UARTprintf("%s", gpi_struct->gpis[index]->name);
        }
    }

    print_index = 0;

    TextCtl::cursor_pos(START_ROW, ADC_NAME_COL);

    TextCtl::text_color(TextCtl::white_text);
    UARTprintf("ADCs (mV)");
    TextCtl::text_color(TextCtl::magenta_text);

    for(uint32_t index=0; index<adc_struct->num_adc_pins; index++) {

        TextCtl::cursor_pos(START_ROW + print_index + 1, ADC_NAME_COL);
        print_index++;
        UARTprintf("%s", adc_struct->adc_pins[index]->name);

    }

    TextCtl::text_color(TextCtl::white_text);

} // End IoCtlPage::draw_page

void IoCtlPage::draw_data(void) {

    uint32_t print_index = 0;


    TextCtl::text_color(TextCtl::yellow_text);

    for(uint32_t index=0; index<gpo_struct->num_gpos; index++) {
        if (gpo_struct->gpos[index]->displayed) {
            TextCtl::cursor_pos(START_ROW + print_index + 1, GPO_DATA_COL);
            print_index++;
            if(gpo_index == index) TextCtl::set_text_mode(TextCtl::mode_reverse);
            UARTprintf("%d", gpo_obj->get(gpo_struct->gpos[index]));
            if(gpo_index == index) TextCtl::set_text_mode(TextCtl::mode_reverse_off);
        }
    }

    print_index=0;

    TextCtl::text_color(TextCtl::cyan_text);

    for(uint32_t index=0; index<gpi_struct->num_gpis; index++) {
        if (gpi_struct->gpis[index]->displayed) {
            TextCtl::cursor_pos(START_ROW + print_index + 1, GPI_DATA_COL);
            print_index++;
            UARTprintf("%d", gpi_obj->get(gpi_struct->gpis[index]));
        }
    }

    print_index = 0;

    TextCtl::text_color(TextCtl::magenta_text);

    for(uint32_t index=0; index<adc_struct->num_adc_pins; index++) {

        TextCtl::cursor_pos(START_ROW + index + 1, ADC_DATA_COL);
        TextCtl::clear_in_line();
        UARTprintf("%d", (uint32_t)((adc_task->get_adc_val(adc_struct->adc_pins[index], adc_task)/4095.0)*3300));

    }

    TextCtl::text_color(TextCtl::white_text);


} // End IoCtlPage::draw_data

void IoCtlPage::draw_input(int character) {

    switch (character) {
    case ArrowKeys::DOWN:
        this->gpo_index = (this->gpo_index + 1) % NUM_OF_DISPLAYED_GPOS;
        break;
    case ArrowKeys::UP:
        if (this->gpo_index == 0) {
            this->gpo_index = NUM_OF_DISPLAYED_GPOS - 1;
        } else {
            this->gpo_index--;
        }
        break;
    case ArrowKeys::RIGHT:
    case ArrowKeys::LEFT :
        if (1 == this->gpo_obj->get(this->gpo_struct->gpos[this->gpo_index])) {
            this->gpo_obj->set(this->gpo_struct->gpos[this->gpo_index], 0);
        } else {
            this->gpo_obj->set(this->gpo_struct->gpos[this->gpo_index], 1);
        }

        break;
    default :
        if (character != 0) {
            this->send_bell();
        }
        break;

    }



} // End IoCtlPage::draw_input

void IoCtlPage::draw_help(void) {

    UARTprintf("Description: \r\n");
    UARTprintf("    This page allows you to manipulate digital outputs, monitor digital inputs,\r\n");
    UARTprintf("and monitor ADC values. These pins only operate at 3v3 regardless of the power\r\n");
    UARTprintf("supply. Observe the pin list page to see which pins are which\r\n\r\n");
    UARTprintf("Operation:\r\n");
    UARTprintf("    Use the up and down arrow keys to select a digital output and left and right\r\n");
    UARTprintf("arrow keys to toggle them.");


}

void IoCtlPage::draw_reset(void) {

    // Disable the interrupt for ADC0 sequence 0 on the processor (NVIC).
    IntDisable(INT_ADC0SS0);
}
