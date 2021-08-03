/*
 * test_class.hpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */

#ifndef TEST_TASK_HPP_
#define TEST_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class TestTask : public ConsolePage {
    public:
        TestTask(void);
    private :
        void task(TestTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;
        uint32_t test;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
    };

#ifdef __cplusplus
}
#endif


#endif /* TEST_TASK_HPP_ */
