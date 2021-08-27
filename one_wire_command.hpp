/*
 * one_wire_command.hpp
 *
 *  Created on: Aug 26, 2021
 *      Author: steph
 */

#ifndef ONE_WIRE_COMMAND_HPP_
#define ONE_WIRE_COMMAND_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "error_logger.hpp"

#include "console_task.hpp"
#include "GPOs.hpp"

typedef enum {
    ONE_WIRE_NO_ERR,
    ONE_WIRE_PULLUPP_ERR,
    ONE_WIRE_NO_RESP_ERR,
}one_wire_pullup_err_e;

typedef enum {
    ONE_WIRE_START,
    ONE_WIRE_RELEASE,
    ONE_WIRE_READ,
    ONE_WIRE_STOP,
}one_wire_write_states_e;

typedef enum {
    ONE_WIRE_IDLE,
    ONE_WIRE_SEND,
    ONE_WIRE_RECEIVE,
    ONE_WIRE_FINISH,
}one_wire_states_e;

typedef enum {
    ENTER_NUM_TX_BYTES,
    ENTER_TX_BYTES,
    ENTER_NUM_RX_BYTES,
    ENTER_MESSAGE,
}one_wire_cmd_states_e;

class OneWireMsg {
public:
    OneWireMsg(void);
    uint8_t* tx_bytes;
    uint8_t* rx_bytes;
    uint32_t num_tx_bytes;
    uint32_t num_rx_bytes;
    uint32_t bytes_txed;
    uint32_t bytes_rxed;
    one_wire_pullup_err_e errors;
};

#ifdef __cplusplus
extern "C" {
#endif

    class OneWireCmd : public ConsolePage {
    public:
        OneWireCmd(GpoObj* gpo_obj);
        void add_msg(OneWireMsg* msg);
    private :
        void task(OneWireCmd* this_ptr);
        static void taskfunwrapper(void* parm);

        void set_timer(uint32_t useconds);

        GpoObj* gpo_obj;
        gpio_pin_t* one_wire_pin;
        QueueHandle_t one_wire_q;
        OneWireMsg* one_wire_msg;
        OneWireMsg* one_wire_cmd_msg;
        one_wire_states_e one_wire_state;
        one_wire_write_states_e one_wire_write_state;
        one_wire_cmd_states_e one_wire_cmd_state;

        uint32_t bit_counter;
        uint32_t byte_counter;
        uint32_t byte_buffer_index;

        void print_errors(OneWireCmd* this_ptr);

        ErrorLogger* logger;
        error_t* pullup_err;
        error_t* no_resp_err;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);


    };

#ifdef __cplusplus
}
#endif




#endif /* ONE_WIRE_COMMAND_HPP_ */
