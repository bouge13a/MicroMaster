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

#include "console_task.hpp"
#include "GPOs.hpp"

typedef enum {
    ONE_WIRE_IDLE,
    ONE_WIRE_SEND,
    ONE_WIRE_RECEIVE,
    ONE_WIRE_FINISH,
}one_wire_states_e;

class OneWireMsg {
public:
    OneWireMsg(void);
    uint8_t* tx_bytes;
    uint8_t* rx_bytes;
    uint32_t num_tx_bytes;
    uint32_t num_rx_bytes;
    uint32_t bytes_txed;
    uint32_t bytes_rxed;
};

#ifdef __cplusplus
extern "C" {
#endif

    class OneWireCmd : public ConsolePage {
    public:
        OneWireCmd(GpoObj* gpo_obj);
    private :
        void task(OneWireCmd* this_ptr);
        static void taskfunwrapper(void* parm);

        GpoObj* gpo_obj;
        gpio_pin_t* one_wire_pin;
        QueueHandle_t one_wire_q;
        OneWireMsg* one_wire_msg;
        OneWireMsg* one_wire_cmd_msg;
        one_wire_states_e one_wire_state;

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
