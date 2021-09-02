/*
 * CAN_sniffer.hpp
 *
 *  Created on: Sep 2, 2021
 *      Author: steph
 */

#ifndef CAN_SNIFFER_HPP_
#define CAN_SNIFFER_HPP_

#include "CAN_command.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "driverlib/can.h"

#include "console_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class CanSniffer : public ConsolePage {
    public:
        CanSniffer(QueueHandle_t can_rx_q,
                   CanCommand* can_command);
    private :
        void task(CanSniffer* this_ptr);
        static void taskfunwrapper(void* parm);

        CanCommand* can_command;
        QueueHandle_t can_rx_q;
        tCANMsgObject* can_rx_msg;

        uint32_t msg_idx;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif

#endif /* CAN_SNIFFER_HPP_ */
