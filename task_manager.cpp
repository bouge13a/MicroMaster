/*
 * task_manager.cpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */



#include "task_manager.hpp"
#include "text_controls.hpp"
#include "uartstdio.h"
#include <assert.h>

static const uint32_t NAME_COL      = 0;
static const uint32_t STATUS_COL    = 20;
static const uint32_t PRIORITY_COL  = 40;
static const uint32_t STACK_COL     = 60;

static const uint32_t START_ROW     = 9;

TaskManager::TaskManager(void): ConsolePage ("Task Manager",
                                             portMAX_DELAY,
                                             false) {

    this->init_once = false;

} // End TaskManager::TaskManager

void TaskManager::draw_page(void) {

    TextCtl::cursor_pos(5, 0);

    UARTprintf("Heap Size:   %d Bytes\r\n", configTOTAL_HEAP_SIZE);
    UARTprintf("Heap Used:   %d Bytes\r\n\n", configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize());

    if (!this->init_once) {
        this->uxArraySize =  uxTaskGetNumberOfTasks();
        this->pxTaskStatusArray = (TaskStatus_t*)pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
        this->init_once = true;
    }

    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

    TextCtl::cursor_pos(START_ROW-1, NAME_COL);
    UARTprintf("Task Name");
    TextCtl::cursor_pos(START_ROW-1, STATUS_COL);
    UARTprintf("Status");
    TextCtl::cursor_pos(START_ROW-1, PRIORITY_COL);
    UARTprintf("Priority");
    TextCtl::cursor_pos(START_ROW-1, STACK_COL);
    UARTprintf("Unused Stack");

    for (uint32_t index=0; index<this->uxArraySize; index++) {

        TextCtl::cursor_pos(index+START_ROW+1, NAME_COL);
        UARTprintf("%s", this->pxTaskStatusArray[index].pcTaskName);

        TextCtl::cursor_pos(index+START_ROW+1, STATUS_COL);
        switch (this->pxTaskStatusArray[index].eCurrentState) {
        case eRunning :
            UARTprintf("R");
            break;
        case eReady :
            UARTprintf("r");
            break;
        case eBlocked :
            UARTprintf("B");
            break;
        case eSuspended :
            UARTprintf("S");
            break;
        case eDeleted :
            UARTprintf("D");
            break;
        case eInvalid :
            UARTprintf("I");
            break;
        default :
            assert(0);
            break;
        }

        TextCtl::cursor_pos(index+START_ROW+1, PRIORITY_COL);
        UARTprintf("%d", this->pxTaskStatusArray[index].uxCurrentPriority);
        TextCtl::cursor_pos(index+START_ROW+1, STACK_COL);
        UARTprintf("%d", this->pxTaskStatusArray[index].usStackHighWaterMark);

    }


} // End TaskManager::draw_page

void TaskManager::draw_data(void) {

} // End TaskManager::draw_data

void TaskManager::draw_input(int character) {

} // End TaskManager::draw_input

void TaskManager::draw_help(void) {

} // End TaskManager::draw_data

void TaskManager::draw_reset(void) {

} // End TaskManager::draw_data
