/*
 * ADC_task.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: steph
 */

#include <string.h>
#include <assert.h>

#include "ADC_task.hpp"
#include "pins_ADC.hpp"

#include "FreeRTOS.h"
#include "task.h"

#include "driverlib/inc/hw_ints.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

static SemaphoreHandle_t adc0_smphr = NULL;

static void ADCSeq0Handler(void) {

    BaseType_t xHigherPriorityTaskWoken, xResult;

    // xHigherPriorityTaskWoken must be initialised to pdFALSE.
    xHigherPriorityTaskWoken = pdFALSE;

    // Clear the Interrupt Flag.
    ADCIntClear(ADC0_BASE, 0);

    xResult = xSemaphoreGiveFromISR( adc0_smphr, &xHigherPriorityTaskWoken );

    // Was the message posted successfully?
    if( xResult != pdFAIL ) {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

} // End ADCSeq0Handler

AdcTask::AdcTask(void) {

    int idx;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "ADC",                                    /* Text name for the task. */
                200,                                          /* Stack size in words, not bytes. */
                this,                                         /* Parameter passed into the task. */
                3,                                            /* Priority at which the task is created. */
                &this->task_handle );                         /* Used to pass out the created task's handle. */


    adc0_smphr = xSemaphoreCreateBinary();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    ADCSequenceDisable(ADC0_BASE, 0);

    for(idx=0; idx<adc_pin_struct.num_adc_pins; idx++) {

        ADCSequenceConfigure(adc_pin_struct.adc_pins[idx]->adc_base,
                             adc_pin_struct.adc_pins[idx]->sequencer,
                             ADC_TRIGGER_PROCESSOR,
                             1);

        SysCtlPeripheralEnable(adc_pin_struct.adc_pins[idx]->gpio_peripheral);

        SysCtlGPIOAHBEnable(adc_pin_struct.adc_pins[idx]->gpio_peripheral);

        GPIOPinTypeADC(adc_pin_struct.adc_pins[idx]->gpio_base,
                       adc_pin_struct.adc_pins[idx]->gpio_pin);

        ADCSequenceStepConfigure(adc_pin_struct.adc_pins[idx]->adc_base,
                                 adc_pin_struct.adc_pins[idx]->sequencer,
                                 adc_pin_struct.adc_pins[idx]->step,
                                 adc_pin_struct.adc_pins[idx]->channel);

    }

    ADCSequenceStepConfigure(ADC0_BASE,
                             0,
                             adc_pin_struct.adc_pins[adc_pin_struct.num_adc_pins-1]->step,
                             adc_pin_struct.adc_pins[adc_pin_struct.num_adc_pins-1]->channel | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 0);

    ADCIntRegister(ADC0_BASE, 0, ADCSeq0Handler);

    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    ADCIntClear(ADC0_BASE, 0);

    // Set the priority "below" max syscall because of FR ISR routine
    IntPrioritySet(INT_ADC0SS0, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);

    // Enable the ADC 0 sample sequence 0 interrupt.
    ADCIntEnable(ADC0_BASE, 0);

    // Enable the interrupt for ADC0 sequence 0 on the processor (NVIC).
    IntEnable(INT_ADC0SS0);

} // End AdcTask::AdcTask

adc_pin_t* AdcTask::get_adc_config(const char* name) {

    int idx;

    for(idx=0; idx<adc_pin_struct.num_adc_pins; idx++) {

        if(0 == strcmp(name, adc_pin_struct.adc_pins[idx]->name)){
            return adc_pin_struct.adc_pins[idx];
        }
    }

    assert(0);
    return NULL;

} // End AdcTask::get_adc_config

uint32_t AdcTask::get_adc_val(adc_pin_t* config, AdcTask* this_ptr) {

    return this_ptr->adc00_step_values[config->step];

} // End AdcTask::get_adc_val

adc_pins_t* AdcTask::get_adc_struct(void) {
    return &adc_pin_struct;
} // End AdcTask::get_adc_struct

void AdcTask::taskfunwrapper(void* parm){
    (static_cast<AdcTask*>(parm))->task((AdcTask*)parm);
} // End AdcTask::taskfunwrapper

void AdcTask::task(AdcTask* this_ptr) {

    // Trigger the ADC conversion.
    ADCProcessorTrigger(ADC0_BASE, 0);

    while(1) {

        xSemaphoreTake(adc0_smphr, portMAX_DELAY);

        ADCSequenceDataGet(ADC0_BASE, 0, this_ptr->adc00_step_values);

        vTaskDelay(100);

        ADCProcessorTrigger(ADC0_BASE, 0);

    }

} // End AdcTask::task
