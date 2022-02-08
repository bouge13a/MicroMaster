/*
 * task_manager.hpp
 *
 *  Created on: Aug 2, 2021
 *      Author: steph
 */

#ifndef TASK_MANAGER_HPP_
#define TASK_MANAGER_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class TaskManager : public ConsolePage {
    public:
        TaskManager(void);
    private :

        char* write_buffer;

        TaskStatus_t *pxTaskStatusArray;
        volatile UBaseType_t uxArraySize, x;
        uint32_t ulTotalRunTime, ulStatsAsPercentage;

        bool init_once;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_reset(void);
        void draw_help(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* TASK_MANAGER_HPP_ */
