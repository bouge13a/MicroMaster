/*
 * no_booster_board.cpp
 *
 *  Created on: Jul 20, 2021
 *      Author: steph
 */
#include <console_uart.hpp>
#include <stdint.h>

#include "test_task.hpp"
#include "console_task.hpp"
#include "no_booster_board.hpp"
#include "I2C_task.hpp"
#include "board_pins.hpp"
#include "error_logger.hpp"
#include "I2C_monitor.hpp"
#include "task_manager.hpp"
#include "I2C_sniffer.hpp"

NoBoosterPack::NoBoosterPack(void) {

    QueueHandle_t uart_rx_queue = xQueueCreate(1000, sizeof(uint8_t));

    UartTask* console_uart = new UartTask(uart_rx_queue);

    ConsoleTask* console_task = new ConsoleTask(uart_rx_queue);

    I2cTask* i2c_cmd_task = new I2cTask(&i2c0);

    I2cMonitorTask* i2c_monitor_task = new I2cMonitorTask(i2c_cmd_task, *i2c_cmd_task->get_vector());

    I2cSnifferTask* i2c_sniffer_task = new I2cSnifferTask(i2c_cmd_task);

    TaskManager* task_manager = new TaskManager();

    ErrorLogger* error_logger = ErrorLogger::get_instance();

    TestTask* test_task = new TestTask();

    console_task->add_page(i2c_sniffer_task);
    console_task->add_page(i2c_cmd_task);
    console_task->add_page(i2c_monitor_task);
    console_task->add_page(error_logger);
    console_task->add_page(task_manager);
    console_task->add_page(test_task);


} // End init_no_booster_board
