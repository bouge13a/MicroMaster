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
    UART_MIDI_MODE,
}uart_stream_mode_e;

typedef enum {
    MIDI_BYTE_0,
    MIDI_BYTE_1,
    MIDI_BYTE_2,
}midi_bytes_e;

typedef struct {
    uint8_t status_bit   : 1;
    uint8_t message_type : 3;
    uint8_t channel      : 4;
    uint8_t key          : 8;
    uint8_t value        : 8;
}midi_msg_t;

typedef union {
    midi_msg_t midi_msg;
    uint8_t midi_bytes[3];
}midi_msg_u;

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
        void process_midi_msg(midi_msg_u* midi_msg);

        UartCmd* uart_cmd;

        midi_msg_u midi_bytes;
        midi_bytes_e midi_byte;


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
