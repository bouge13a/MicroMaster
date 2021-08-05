/*
 * utils.c
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */
#include <utils.hpp>

void int_to_bin_string(uint32_t integer, uint8_t* bin_string) {


    uint32_t index = 0;
    uint32_t int_buff = integer;

    for (index=0; index<8; index++) {
        int_buff = int_buff & 1;
        if (int_buff) {
            bin_string[7-index] = '1';
        } else {
            bin_string[7-index] = '0';
        }
        int_buff = integer >> index + 1;
    }

    bin_string[8] = '\0';

} // End int_to_bin_string

