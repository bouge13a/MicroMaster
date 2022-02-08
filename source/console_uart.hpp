/*
 * uart_task.hpp
 *
 *  Created on: Jul 25, 2021
 *      Author: steph
 */

#ifndef CONSOLE_UART_HPP_
#define CONSOLE_UART_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

int get_char(void);

#ifdef __cplusplus
extern "C" {
#endif

    class UartTask {
    public:
        UartTask(QueueHandle_t uart_rx_queue);

    };

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_UART_HPP_ */
