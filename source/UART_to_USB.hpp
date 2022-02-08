/*
 * UART_to_USB.hpp
 *
 *  Created on: Aug 25, 2021
 *      Author: steph
 */

#ifndef UART_TO_USB_HPP_
#define UART_TO_USB_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

int get_char(void);

#ifdef __cplusplus
extern "C" {
#endif

    class UART_to_USB {
    public:
        UART_to_USB(QueueHandle_t uart_rx_que,
                    QueueHandle_t uart_tx_que);
    private :
        void task(UART_to_USB* this_ptr);
        static void taskfunwrapper(void* parm);

    };

#ifdef __cplusplus
}
#endif

#endif /* UART_TO_USB_HPP_ */
