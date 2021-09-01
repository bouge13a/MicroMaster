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

#include "driverlib/can.h"

#include "console_task.hpp"

typedef enum {
    CAN_RX_MESSAGE_OBJ = 1,
    CAN_TX_MESSAGE_OBJ,
}can_msg_obj_e;

#ifdef __cplusplus
extern "C" {
#endif

    class CanCommand : public ConsolePage {
    public:
        CanCommand(void);
    private :
        void tx_task(CanCommand* this_ptr);
        static void tx_taskfunwrapper(void* parm);

        void rx_task(CanCommand* this_ptr);
        static void rx_taskfunwrapper(void* parm);


        QueueHandle_t can_tx_q;

        tCANMsgObject can_rx_msg;
        tCANMsgObject can_tx_msg;

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
