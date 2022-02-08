/*
 * utils.c
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */
#include <utils.hpp>
#include <math.h>

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

uint32_t ascii_to_hex(uint8_t character) {

    if (character >= '0' && character <='9') {
        return character - '0';
    }

    if ((character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')) {
        return character - 'a' + 10;
    }

    return 0;
} // End SpiCmdTask::ascii_to_hex

uint32_t octalToDecimal(int octalNum){
  int decimalNum = 0, i = 0;
  while(octalNum != 0){
    decimalNum += (octalNum%10) * pow(8,i);
    ++i;
    octalNum/=10;
    }
  i = 1;
  return decimalNum;
}

int decimalToOctal(int decimalNumber)
{
    int rem, i = 1, octalNumber = 0;
    while (decimalNumber != 0)
    {
        rem = decimalNumber % 8;
        decimalNumber /= 8;
        octalNumber += rem * i;
        i *= 10;
    }
    return octalNumber;
}


