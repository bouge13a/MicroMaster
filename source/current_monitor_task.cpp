/*
 * current_monitor_task.cpp
 *
 *  Created on: Jul 16, 2022
 *      Author: steph
 */
#include <console_uart.hpp>
#include "uartstdio.h"
#include "current_monitor_task.hpp"
#include "text_controls.hpp"
#include "I2C_aux.hpp"
#include "utils.hpp"

static const float CURRENT_DIV = 10.0;

static const uint32_t UPDATE_RATE = 100;

static uint8_t INA219_ADDRESS = 0x40u;                       // 1000000 (A0+A1=GND)

static uint8_t INA_CALIB_DATA[]   = {0x05u, 0x10u, 0x00u};
static uint8_t INA_CONFIG_DATA[]  = {0x00u, 0x3Du, 0xDFu};
static uint8_t INA_CURRENT_DATA[] = {0x04u};

void CurrentMonitorTask::taskfunwrapper(void* parm){
    (static_cast<CurrentMonitorTask*>(parm))->task((CurrentMonitorTask*)parm);
} // End CurrentMonitorTask::taskfunwrapper

CurrentMonitorTask::CurrentMonitorTask(I2cAux* i2c,
                                       OLED_GFX* oled_gfx) : ConsolePage("Current",
                                                                  100,
                                                                  false) {

    this->oled_gfx_obj = oled_gfx;
    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I monitor",                                     /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                this,                                      /* Parameter passed into the task. */
                3,                                         /* Priority at which the task is created. */
                NULL);

    this->i2c = i2c;

    this->calibration_msg = new I2cMsgAux();
    this->config_msg = new I2cMsgAux();
    this->current_msg = new I2cMsgAux();

    this->calibration_msg->tx_data = INA_CALIB_DATA;
    this->calibration_msg->address = INA219_ADDRESS;
    this->calibration_msg->num_tx_bytes = 3;
    this->calibration_msg->num_rx_bytes = 0;
    this->calibration_msg->semphr = nullptr;


    this->config_msg->tx_data = INA_CONFIG_DATA;
    this->config_msg->address = INA219_ADDRESS;
    this->config_msg->num_tx_bytes = 3;
    this->config_msg->num_rx_bytes = 0;
    this->config_msg->semphr = nullptr;

    this->current_msg->tx_data = INA_CURRENT_DATA;
    this->current_msg->address = INA219_ADDRESS;
    this->current_msg->num_tx_bytes = 1;
    this->current_msg->num_rx_bytes = 2;
    this->current_msg->rx_data = this->raw_current;
    this->current_msg->semphr = xSemaphoreCreateBinary();

} // End CurrentMonitorTask::CurrentMonitorTask


void CurrentMonitorTask::task(CurrentMonitorTask* this_ptr) {

    this_ptr->i2c->add_i2c_msg(this_ptr->calibration_msg);
    this_ptr->i2c->add_i2c_msg(this_ptr->config_msg);

    while(1){

        this_ptr->i2c->add_i2c_msg(this_ptr->current_msg);
        xSemaphoreTake(this_ptr->current_msg->semphr, portMAX_DELAY);
        this_ptr->set_update_pending(true);
        vTaskDelay(UPDATE_RATE);

    }

} // End CurrentMonitorTask::task

void CurrentMonitorTask::draw_page(void) {



}
void CurrentMonitorTask::draw_data(void) {

    int16_t current = 0u;
    current = this->raw_current[0];
    current |= static_cast<int16_t>(this->raw_current[1]) << 8;
    UARTprintf("%4d\r", current);

}
void CurrentMonitorTask::draw_input(int character) {

}

void CurrentMonitorTask::draw_reset(void) {

}

void CurrentMonitorTask::draw_help(void) {

    UARTprintf("THIS IS A HELP PAGE\r\n");

}

void CurrentMonitorTask::update_display(void) {

    char current_str[8];
    float current = (raw_current[0] | (uint16_t)(raw_current[1] << 8))/CURRENT_DIV;
    ftoa(current, current_str, 1);
    oled_gfx_obj->oled->send_str_xy(current_str, 2, 12);

} // End CurrentMonitorTask::update_display

