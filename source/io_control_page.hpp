/*
 * io_control_page.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef IO_CONTROL_PAGE_HPP_
#define IO_CONTROL_PAGE_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"
#include "GPOs.hpp"
#include "GPIs.hpp"
#include "ADC_task.hpp"
#include "board_pin_defs.hpp"

class IoCtlPage : public ConsolePage {
public:
    IoCtlPage(GpoObj* gpo_obj,
              GpiObj* gpi_obj,
              AdcTask* adc_obj);

    gpos_t* gpo_struct;
    gpis_t* gpi_struct;
    adc_pins_t* adc_struct;

private :

    GpoObj* gpo_obj;
    GpiObj* gpi_obj;
    AdcTask* adc_task;

    uint32_t gpo_index;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_help(void);
    void draw_reset(void);
};





#endif /* IO_CONTROL_PAGE_HPP_ */
