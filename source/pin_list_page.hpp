/*
 * pin_list.hpp
 *
 *  Created on: Feb 11, 2022
 *      Author: steph
 */

#ifndef PIN_LIST_HPP_
#define PIN_LIST_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class PinPage : public ConsolePage {
    public:
        PinPage(void);
    private :

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* PIN_LIST_HPP_ */
