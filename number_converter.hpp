/*
 * number_converter.hpp
 *
 *  Created on: Sep 23, 2021
 *      Author: steph
 */

#ifndef NUMBER_CONVERTER_HPP_
#define NUMBER_CONVERTER_HPP_

#include "console_task.hpp"

class NumConverter : public ConsolePage {
public:
    NumConverter(void);

private :
    void draw_menu(uint32_t index);
    void draw_numbers(uint32_t number);

    uint32_t menu_index;
    uint32_t byte_buffer_idx;
    uint32_t num_buffer;
    char* bin_string;
    char* byte_buffer;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_help(void);
    void draw_reset(void);

};


#endif /* NUMBER_CONVERTER_HPP_ */
