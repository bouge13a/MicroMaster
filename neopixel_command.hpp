/*
 * neopixel_control.hpp
 *
 *  Created on: Oct 8, 2021
 *      Author: steph
 */

#ifndef NEOPIXEL_CONTROL_HPP_
#define NEOPIXEL_CONTROL_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "console_task.hpp"
#include "GPOs.hpp"

typedef enum {
    NEOPIX_IDLE,
    NEOPIX_SEND,
    NEOPIX_SEND_STREAM,
    NEOPIX_FINISH,
}neopix_states_e;

typedef enum {
    neopix_idle,
    neopix_processing,
    neopix_finished,
}neopix_msg_states_e;

typedef enum {
    neopix_command_msg,
    neopix_normal_msg,
    neopix_clear_msg,
    neopix_streamer_msg,
    neopixel_rainbow_msg,
}neopix_msg_types_e;

typedef enum {
    NEOPIX_CMD_GET_NUM_TX_MSGS,
    NEOPIX_CMD_GET_TX_MSGS,
    NEOPIX_SEND_MSG,
}neopix_cmd_states_e;

class NeopixMsg {
public:
    NeopixMsg(neopix_msg_types_e msg_type);
    uint32_t num_tx_msgs;
    uint32_t* tx_msgs;
    uint32_t msgs_txed;
    neopix_msg_states_e msg_state;
    neopix_msg_types_e msg_type;
};

#ifdef __cplusplus
extern "C" {
#endif

    class NeopixelCtl : public ConsolePage {
    public:
        NeopixelCtl(void);
        void add_msg(NeopixMsg* msg);
    private :
        void task(NeopixelCtl* this_ptr);
        static void taskfunwrapper(void* parm);

        void set_timer(uint32_t useconds);
        void send_bit(uint32_t bit);

        void init_spi(void);
        void deinit_spi(void);
        void init_gpo(void);
        void deinit_gpo(void);

        void set_gpo(uint32_t value);

        QueueHandle_t neopix_msg_q;
        neopix_states_e neopix_state;
        NeopixMsg* neopix_msg;
        NeopixMsg* neopix_cmd_msg;
        uint32_t bit_counter;
        neopix_cmd_states_e cmd_state;

        uint32_t byte_buffer_index;
        uint8_t* byte_buffer;
        uint32_t msg_counter;
        uint32_t stream_counter;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* NEOPIXEL_CONTROL_HPP_ */
