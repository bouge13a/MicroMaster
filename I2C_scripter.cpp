/*
 * I2C_scripter.cpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */

#include "I2C_scripter.hpp"


#include "text_controls.hpp"
#include "uartstdio.h"
#include <assert.h>


void I2cScripterTask::taskfunwrapper(void* parm){
    (static_cast<I2cScripterTask*>(parm))->task((I2cScripterTask*)parm);
} // End I2cScripterTask::taskfunwrapper


I2cScripterTask::I2cScripterTask(I2cTask* i2c) : ConsolePage("I2C Scripter",
                                                            portMAX_DELAY,
                                                            true) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "I2C script",         /* Text name for the task. */
                200,                  /* Stack size in words, not bytes. */
                this,                 /* Parameter passed into the task. */
                3,                    /* Priority at which the task is created. */
                NULL);

    this->i2c = i2c;

    this-> send_semphr = xSemaphoreCreateBinary();

    this->i2c_msg = new I2cMsg(normal_msg);

    this->buffer = new char[50];

} // End I2cScripterTask::I2cScripterTask


void I2cScripterTask::task(I2cScripterTask* this_ptr) {

    while(1){

        xSemaphoreTake(this->send_semphr, portMAX_DELAY);


    }

} // End I2cScripterTask::task

void I2cScripterTask::draw_page(void) {


}

void I2cScripterTask::draw_data(void) {

}

void I2cScripterTask::draw_input(int character) {

    if ('\r' == character) {
        xSemaphoreGive(this->send_semphr);
    }


}
