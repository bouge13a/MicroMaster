/*
 * menu_constants.hpp
 *
 *  Created on: Aug 4, 2021
 *      Author: steph
 */

#ifndef MENU_CONSTANTS_HPP_
#define MENU_CONSTANTS_HPP_


const char* i2c_speed_menu[] = {"100kbs",
                                "400kbs",
                                "1Mbs"};
const char* i2c_speed_name = "I2C Speed\0";
const uint32_t i2c_speed_num = 3;


const char* spi_mode_menu[] = {"Mode 0",
                               "Mode 1",
                               "Mode 2",
                               "Mode 3",
                               "TI",
                               "MicroWire"};
const char* spi_mode_name = "SPI Mode";
const uint32_t spi_mode_num = 6;


const char* uart_parity_menu[] = {"None",
                                  "Even",
                                  "Odd",
                                  "One",
                                  "Zero"};
const char* uart_parity_name = "UART Parity";
const uint32_t uart_parity_num = 5;


const char* uart_stop_bit_menu[] = {"One",
                                    "Two"};
const char* uart_stop_bit_name = "UART Stop Bit";
const uint32_t uart_stop_bit_num = 2;


const char* uart_mode_menu[] = {"Text",
                                "Byte",
                                "MIDI"};
const char* uart_mode_name = "UART Stream";
const uint32_t uart_mode_num = 3;


const char* power_on_menu[] = {"Off",
                               "On"};
const char* power_on_name = "Power Supply";
const uint32_t power_on_num = 2;


const char* pullup_en_menu[] = {"On",
                                "Off"};
const char* pullup_en_name = "I2C Pullups";
const uint32_t pullup_en_num = 2;


const char* pwm_mode_menu[] = {"Signal",
                                "Servo"};
const char* pwm_mode_name = "PWM Mode";
const uint32_t pwm_mode_num = 2;

#endif /* MENU_CONSTANTS_HPP_ */
