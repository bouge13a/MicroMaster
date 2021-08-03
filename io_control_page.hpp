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
#include "Adc_task.hpp"
#include "board_pin_defs.hpp"

class IoCtlPage : public ConsolePage {
public:
    IoCtlPage(GpoObj* gpo_obj,
              GpiObj* gpi_obj,
              AdcTask* adc_obj);

private :

    GpoObj* gpo_obj;
    GpiObj* gpi_obj;
    AdcTask* adc_task;

    gpos_t* gpo_struct;
    gpis_t* gpi_struct;
    adc_pins_t* adc_struct;

    uint32_t gpo_index;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
};





#endif /* IO_CONTROL_PAGE_HPP_ */
