/*
 * no_booster_board.cpp
 *
 *  Created on: Jul 20, 2021
 *      Author: steph
 */
#include <console_uart.hpp>
#include <initialization.hpp>
#include <I2C_search.hpp>
#include <pins_I2C.hpp>
#include <stdint.h>
#include <UART_to_USB.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "test_task.hpp"
#include "console_task.hpp"
#include "I2C_task.hpp"
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
#include "one_wire_search.hpp"
#include "CAN_command.hpp"
#include "CAN_sniffer.hpp"
#include "I2C_sniffer.hpp"
#include "number_converter.hpp"
#include "FTDI_emulator.hpp"
#include "neopixel_command.hpp"
#include "neopixel_rgb.hpp"
#include "neopixel_menu.hpp"
#include "pin_list_page.hpp"
#include "I2C_aux.hpp"
#include "current_monitor_task.hpp"
#include "display_task.hpp"

static ConsoleTask* console_task = nullptr;
static uint32_t power_idx = 0;
static display_tools_t display_tools;
DisplayTask* display_task = nullptr;

PreScheduler::PreScheduler(void) {

    QueueHandle_t uart_rx_queue = xQueueCreate(100, sizeof(uint8_t));
    QueueHandle_t uart_tx_queue = xQueueCreate(100, sizeof(uint8_t));

    set_uart_tx_q(uart_tx_queue);

    UART_to_USB* uart_to_usb = new UART_to_USB(uart_rx_queue,
                                               uart_tx_queue);

    console_task = new ConsoleTask(uart_rx_queue,
                                   &power_idx);

    SemaphoreHandle_t i2c_aux_sem = xSemaphoreCreateBinary();

    I2cAux* i2c_aux = new I2cAux(&i2c3);

    display_tools.i2c_sem = i2c_aux_sem;
    display_tools.i2c = i2c_aux;

    display_task = new DisplayTask();

} // End init_no_booster_board

PostScheduler::PostScheduler(void) {

    QueueHandle_t can_rx_q = xQueueCreate(2, sizeof(tCANMsgObject*));

    I2cTask* i2c_cmd_task = new I2cTask(&i2c0);

    I2cMonitorTask* i2c_monitor_task = new I2cMonitorTask(i2c_cmd_task, *i2c_cmd_task->get_vector());

    I2cSnifferTask* i2c_sniffer_task = new I2cSnifferTask(i2c_cmd_task);

    I2cScripterTask* i2c_scripter_task = new I2cScripterTask(i2c_cmd_task);

    TaskManager* task_manager = new TaskManager();

    ErrorLogger* error_logger = ErrorLogger::get_instance();

    GpoObj* gpo_obj = new GpoObj(power_idx);

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

    OneWireSearch* one_wire_search = new OneWireSearch(gpo_obj,
                                                       one_wire_command);

    CanCommand* can_command = new CanCommand(can_rx_q);

    CanSniffer* can_sniffer = new CanSniffer(can_rx_q,
                                             can_command);

    I2cSniffer* i2c_sniff = new I2cSniffer(gpi_obj);

    NumConverter* num_converter = new NumConverter();

    PinPage* pin_page = new PinPage();

    CurrentMonitorTask* current_monitor_task = new CurrentMonitorTask(display_tools.i2c);

    menu_page->add_menu_row(new MenuRow(power_on_num,
                                        set_power_supplies,
                                        power_on_menu,
                                        power_on_name));

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

    menu_page->add_menu_row(new MenuRow(pwm_mode_num,
                                        set_pwm_mode,
                                        pwm_mode_menu,
                                        pwm_mode_name));

    console_task->add_page(menu_page);
    console_task->add_page(io_control_page);
    console_task->add_page(i2c_sniffer_task);
    console_task->add_page(i2c_scripter_task);
    console_task->add_page(i2c_cmd_task);
    console_task->add_page(i2c_monitor_task);
    console_task->add_page(i2c_sniff);
    console_task->add_page(spi_command);
    console_task->add_page(spi_monitor);
    console_task->add_page(uart_cmd);
    console_task->add_page(uart_streamer);
    console_task->add_page(one_wire_command);
    console_task->add_page(one_wire_search);
    console_task->add_page(can_command);
    console_task->add_page(can_sniffer);
    console_task->add_page(pwm_page);
    console_task->add_page(num_converter);
    console_task->add_page(pin_page);
    console_task->add_page(current_monitor_task);
    console_task->add_page(error_logger);
    console_task->add_page(task_manager);

    display_task->add_display_update(current_monitor_task);
    display_task->add_display_update(menu_page);

}

FtdiProgram::FtdiProgram(void) {

    FtdiEmulator* ftdi_emulator = new FtdiEmulator();

}

NeopixelSuite::NeopixelSuite(void) {


    NeopixelCtl* neopixel_command = new NeopixelCtl();
    NeopixelMenu* neopixel_menu = new NeopixelMenu(neopixel_command);
    NeopixelRgb* neopixel_rgb = new NeopixelRgb(neopixel_command);
    NumConverter* num_converter = new NumConverter();
    TaskManager* task_manager = new TaskManager();
    new I2cAux(&i2c3);

    console_task->add_page(neopixel_command);
    console_task->add_page(neopixel_rgb);
    console_task->add_page(neopixel_menu);
    console_task->add_page(num_converter);
    console_task->add_page(task_manager);
}
