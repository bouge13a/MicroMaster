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
#include "I2C_scripter.hpp"
#include "GPOs.hpp"
#include "GPIs.hpp"
#include "ADC_task.hpp"
#include "io_control_page.hpp"
#include "menu_page.hpp"
#include "menu_constants.hpp"
#include "PWM_page.hpp"
#include "SPI_command.hpp"


NoBoosterPack::NoBoosterPack(void) {

    QueueHandle_t uart_rx_queue = xQueueCreate(1000, sizeof(uint8_t));

    UartTask* console_uart = new UartTask(uart_rx_queue);

    ConsoleTask* console_task = new ConsoleTask(uart_rx_queue);

    I2cTask* i2c_cmd_task = new I2cTask(&i2c0);

    I2cMonitorTask* i2c_monitor_task = new I2cMonitorTask(i2c_cmd_task, *i2c_cmd_task->get_vector());

    I2cSnifferTask* i2c_sniffer_task = new I2cSnifferTask(i2c_cmd_task);

    I2cScripterTask* i2c_scripter_task = new I2cScripterTask(i2c_cmd_task);

    TaskManager* task_manager = new TaskManager();

    ErrorLogger* error_logger = ErrorLogger::get_instance();

    TestTask* test_task = new TestTask();

    GpoObj* gpo_obj = new GpoObj();

    GpiObj* gpi_obj = new GpiObj();

    AdcTask* adc_task = new AdcTask();

    IoCtlPage* io_control_page = new IoCtlPage(gpo_obj,
                                               gpi_obj,
                                               adc_task);

    PWMpage* pwm_page = new PWMpage();

    MenuOptions* menu_page = new MenuOptions();

    SpiCmdTask* spi_command = new SpiCmdTask();


    menu_page->add_menu_row(new MenuRow(i2c_speed_num,
                                        set_i2c_clock_speed,
                                        i2c_speed_menu,
                                        i2c_speed_name));

    menu_page->add_menu_row(new MenuRow(i2c_speed_num,
                                        set_i2c_clock_speed,
                                        i2c_speed_menu,
                                        i2c_speed_name));

    console_task->add_page(menu_page);
    console_task->add_page(io_control_page);
    console_task->add_page(i2c_sniffer_task);
    console_task->add_page(i2c_scripter_task);
    console_task->add_page(i2c_cmd_task);
    console_task->add_page(i2c_monitor_task);
    console_task->add_page(spi_command);
    console_task->add_page(pwm_page);
    console_task->add_page(error_logger);
    console_task->add_page(task_manager);
    console_task->add_page(test_task);


} // End init_no_booster_board
