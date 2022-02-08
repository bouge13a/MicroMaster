/*
 * GPIs.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef GPIS_HPP_
#define GPIS_HPP_

#include <stdint.h>
#include "board_pin_defs.hpp"

class GpiObj {
public:
    GpiObj(void);
    gpio_pin_t* get_config(const char* name);
    uint32_t get(gpio_pin_t* config);
    gpis_t* get_struct(void);
private:
    gpis_t* gpi_info;
};




#endif /* GPIS_HPP_ */
