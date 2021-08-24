/*
 * UART_streamer.hpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */

#ifndef UART_STREAMER_HPP_
#define UART_STREAMER_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"



#ifdef __cplusplus
extern "C" {
#endif

    class UartStreamer : public ConsolePage {
    public:
        UartStreamer(void);
    private :
        void task(UartStreamer* this_ptr);
        static void taskfunwrapper(void* parm);

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif



#endif /* UART_STREAMER_HPP_ */
