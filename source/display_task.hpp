/*
 * display_task.hpp
 *
 *  Created on: Jul 17, 2022
 *      Author: steph
 */

#ifndef DISPLAY_TASK_HPP_
#define DISPLAY_TASK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <vector>

class DisplayUpdate {
public:
    DisplayUpdate(void);
    virtual void update_display(void) = 0;
    bool is_update_pending(void);
    void set_update_pending(bool update_pend);
private:
    bool update_pending;
};

#ifdef __cplusplus
extern "C" {
#endif

    class DisplayTask {
    public:
        DisplayTask(void);
        void add_display_update(DisplayUpdate* display_update);
    private :
        void task(DisplayTask* this_ptr);
        static void taskfunwrapper(void* parm);
        void init_display(void);

        std::vector<DisplayUpdate*> display_updates;
    };

#ifdef __cplusplus
}
#endif





#endif /* DISPLAY_TASK_HPP_ */
