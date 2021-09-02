/*
 * no_booster_board.cpp
 *
 *  Created on: Jul 20, 2021
 *      Author: steph
 */
#include <console_uart.hpp>
#include <I2C_search.hpp>
#include <stdint.h>
#include <UART_to_USB.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "test_task.hpp"
#include "console_task.hpp"
#include "no_booster_board.hpp"
#include "I2C_task.hpp"
#include "board_pins.hpp"
#include "error_logger.hpp"
#include "I2C_monitor.hpp"
#include "task_manager.hpp"
#include "I2C_scripter.hpp"
#include "GPOs.hpp"
#include "GPIs.hpp"
#include "ADC_task.hpp"
#include "io_control_page.hpp"
#include "menu_page.hpp"
#include "menu_constants.hpp"
#include "PWM_page.hpp"
#include "SPI_command.hpp"
#include "SPI_monitor.hpp"
#include "UART_streamer.hpp"
#include "UART_command.hpp"
#include "uartstdio.h"
#include "one_wire_command.hpp"
#include "CAN_command.hpp"
#include "CAN_sniffer.hpp"

NoBoosterPack::NoBoosterPack(void) {

    QueueHandle_t uart_rx_queue = xQueueCreate(100, sizeof(uint8_t));
    QueueHandle_t uart_tx_queue = xQueueCreate(100, sizeof(uint8_t));

    QueueHandle_t can_rx_q = xQueueCreate(2, sizeof(tCANMsgObject*));

    set_uart_tx_q(uart_tx_queue);

    UART_to_USB* uart_to_usb = new UART_to_USB(uart_rx_queue,
                                               uart_tx_queue);

    ConsoleTask* console_task = new ConsoleTask(uart_rx_queue);

    I2cTask* i2c_cmd_task = new I2cTask(&i2c0);

    I2cMonitorTask* i2c_monitor_task = new I2cMonitorTask(i2c_cmd_task, *i2c_cmd_task->get_vector());

    I2cSnifferTask* i2c_sniffer_task = new I2cSnifferTask(i2c_cmd_task);

    I2cScripterTask* i2c_scripter_task = new I2cScripterTask(i2c_cmd_task);

    TaskManager* task_manager = new TaskManager();

    ErrorLogger* error_logger = ErrorLogger::get_instance();

    //TestTask* test_task = new TestTask();

    GpoObj* gpo_obj = new GpoObj();

    GpiObj* gpi_obj = new GpiObj();

    AdcTask* adc_task = new AdcTask();

    IoCtlPage* io_control_page = new IoCtlPage(gpo_obj,
                                               gpi_obj,
                                               adc_task);

    PWMpage* pwm_page = new PWMpage();

    MenuOptions* menu_page = new MenuOptions();

    SpiCmdTask* spi_command = new SpiCmdTask();

    SpiMonitorTask* spi_monitor = new SpiMonitorTask(spi_command, *spi_command->get_vector());

    UartCmd* uart_cmd = new UartCmd();

    UartStreamer* uart_streamer = new UartStreamer(uart_cmd);

    OneWireCmd* one_wire_command = new OneWireCmd(gpo_obj);

    CanCommand* can_command = new CanCommand(can_rx_q);

    CanSniffer* can_sniffer = new CanSniffer(can_rx_q,
                                             can_command);

    menu_page->add_menu_row(new MenuRow(power_on_num,
                                        set_power_supplies,
                                        power_on_menu,
                                        power_on_name));

    menu_page->add_menu_row(new MenuRow(power_select_num,
                                        sel_power_supply,
                                        power_select_menu,
                                        power_select_name));

    menu_page->add_menu_row(new MenuRow(pullup_en_num,
                                        set_pullup_en,
                                        pullup_en_menu,
                                        pullup_en_name));

    menu_page->add_menu_row(new MenuRow(i2c_speed_num,
                                        set_i2c_clock_speed,
                                        i2c_speed_menu,
                                        i2c_speed_name));

    menu_page->add_menu_row(new MenuRow(spi_mode_num,
                                        spi_set_mode,
                                        spi_mode_menu,
                                        spi_mode_name));

    menu_page->add_menu_row(new MenuRow(uart_parity_num,
                                        uart_set_parity,
                                        uart_parity_menu,
                                        uart_parity_name));

    menu_page->add_menu_row(new MenuRow(uart_stop_bit_num,
                                        uart_set_stop_bits,
                                        uart_stop_bit_menu,
                                        uart_stop_bit_name));

    menu_page->add_menu_row(new MenuRow(uart_mode_num,
                                        set_uart_stream_mode,
                                        uart_mode_menu,
                                        uart_mode_name));



    console_task->add_page(menu_page);
    console_task->add_page(io_control_page);
    console_task->add_page(i2c_sniffer_task);
    console_task->add_page(i2c_scripter_task);
    console_task->add_page(i2c_cmd_task);
    console_task->add_page(i2c_monitor_task);
    console_task->add_page(spi_command);
    console_task->add_page(spi_monitor);
    console_task->add_page(uart_cmd);
    console_task->add_page(uart_streamer);
    console_task->add_page(one_wire_command);
    console_task->add_page(can_command);
    console_task->add_page(can_sniffer);
    console_task->add_page(pwm_page);
    console_task->add_page(error_logger);
    console_task->add_page(task_manager);
    //console_task->add_page(test_task);


} // End init_no_booster_board
