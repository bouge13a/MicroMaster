/*
 * menu_constants.hpp
 *
 *  Created on: Aug 4, 2021
 *      Author: steph
 */

#ifndef MENU_CONSTANTS_HPP_
#define MENU_CONSTANTS_HPP_


//////////////////////////////////////////////////////////////////////
//                         I2C
//////////////////////////////////////////////////////////////////////
const char* i2c_speed_menu[] = {"100kbs\0",
                                "400kbs\0"};
const char* i2c_speed_name = "I2C Speed\0";

const uint32_t i2c_speed_num = 2;

const char* spi_mode_menu[] = {"Mode 0",
                               "Mode 1",
                               "Mode 2",
                               "Mode 3"};

const char* spi_mode_name = "SPI Mode";

const uint32_t spi_mode_num = 4;

#endif /* MENU_CONSTANTS_HPP_ */
