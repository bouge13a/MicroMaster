/*
 * utils.h
 *
 *  Created on: Aug 4, 2020
 *      Author: steph
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <stdint.h>


void int_to_bin_string(uint32_t integer, uint8_t* string);

uint32_t ascii_to_hex(uint8_t character);

uint32_t octalToDecimal(int octalNum);

int decimalToOctal(int decimalNumber);

#endif /* UTILS_HPP_ */
