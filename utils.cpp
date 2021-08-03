/*
 * utils.c
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */
#include <utils.hpp>

void int_to_bin_string(uint32_t integer, uint8_t* bin_string) {


    uint32_t index = 0;

    for (index=0; index<8; index++) {
        if ((1 << index) == ((1 << index) & integer)){
            bin_string[index] = '1';
        } else {
            bin_string[index] = '0';
        }
    }

    bin_string[index] = '\0';

} // End int_to_bin_string

