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
#include "UART_command.hpp"

typedef enum {
    UART_TEXT_MODE,
    UART_BYTE_MODE,
}uart_stream_mode_e;

void set_uart_stream_mode(uint32_t index);

#ifdef __cplusplus
extern "C" {
#endif

    class UartStreamer : public ConsolePage {
    public:
        UartStreamer(UartCmd* uart_cmd);
    private :
        void task(UartStreamer* this_ptr);
        static void taskfunwrapper(void* parm);

        UartCmd* uart_cmd;

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
