/*
 * menu_page.hpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#ifndef MENU_PAGE_HPP_
#define MENU_PAGE_HPP_

#include <stdint.h>
#include <vector>
#include "console_task.hpp"

class MenuRow {
public :
    MenuRow(uint32_t num_of_options,
            void (*callback)(uint32_t index),
            const char** options_names,
            const char* row_name);

    uint32_t num_of_options;
    void (*callback)(uint32_t index);
    const char** options_names;
    const char* row_name;
    uint32_t column_index;
};

class MenuOptions : public ConsolePage {
public:
    MenuOptions(void);
    void add_menu_row(MenuRow* menu_row);

private :

    void print_row(uint32_t row_index);

    std::vector<MenuRow*> menu_rows;

    uint32_t menu_index;


    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
};


#endif /* MENU_PAGE_HPP_ */
