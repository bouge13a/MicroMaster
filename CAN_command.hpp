/*
 * CAN_command.hpp
 *
 *  Created on: Sep 1, 2021
 *      Author: steph
 */

#ifndef CAN_COMMAND_HPP_
#define CAN_COMMAND_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "driverlib/can.h"

#include "console_task.hpp"
#include "error_logger.hpp"

typedef enum {
    CAN_TX_MESSAGE_OBJ = 1,
    CAN_RX_MESSAGE_OBJ = 2,
}can_msg_obj_e;

typedef enum {
    CAN_CMD_ID,
    CAN_CMD_SPEED,
    CAN_CMD_NUM_TX_BYTES,
    CAN_CMD_TX_BYTES,
    CAN_CMD_SEND,
}can_cmd_states_e;

#ifdef __cplusplus
extern "C" {
#endif

    class CanCommand : public ConsolePage {
    public:
        CanCommand(QueueHandle_t can_rx_q);
        void log_print_errors(void);
        void send_last_message(void);
        bool add_can_msg(tCANMsgObject* can_tx_msg_p);
    private :
        void tx_task(CanCommand* this_ptr);
        static void tx_taskfunwrapper(void* parm);

        void rx_task(CanCommand* this_ptr);
        static void rx_taskfunwrapper(void* parm);

        void send_can_message(CanCommand* this_ptr, tCANMsgObject* can_rx_msg_p);

        QueueHandle_t can_tx_q;
        QueueHandle_t can_rx_q;

        tCANMsgObject can_tx_msg;
        tCANMsgObject can_rx_msg;

        tCANMsgObject* can_tx_msg_p;

        can_cmd_states_e can_cmd_state;
        uint32_t byte_buffer_idx;
        uint8_t byte_buffer;
        uint32_t byte_counter;
        uint8_t* speed_buffer;

        bool msg_rdy_flag;

        ErrorLogger* logger;
        error_t* bus_off_err;
        error_t* ewarn_err;
        error_t* epass_err;
        error_t* lec_stuff_err;
        error_t* lec_form_err;
        error_t* lec_ack_err;
        error_t* lec_bit1_err;
        error_t* lec_bit0_err;
        error_t* lec_crc_err;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif



#endif /* CAN_COMMAND_HPP_ */
