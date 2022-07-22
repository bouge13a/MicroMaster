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
#include "OLED_GFX.h"
#include <vector>

typedef enum {
    MAIN_SUITE,
    FTDI_SUITE,
    NEOPIXEL_SUITE,
    PWM_SUITE,
}suite_e;

class DisplayUpdate {
public:
    DisplayUpdate(void);
    virtual void update_display(void) = 0;
    bool is_update_pending(void);
    void set_update_pending(bool update_pend);

private:
    volatile bool update_pending;

};

#ifdef __cplusplus
extern "C" {
#endif

    class DisplayTask {
    public:
        DisplayTask(OLED_GFX* oled_gfx,
                    SemaphoreHandle_t display_sem,
                    uint32_t* power_idx);
        void add_display_update(DisplayUpdate* display_update);
        void set_suite(suite_e suite);
    private :
        void task(DisplayTask* this_ptr);
        static void taskfunwrapper(void* parm);
        void draw_main_suite(void);
        OLED_GFX* oled_gfx;
        SemaphoreHandle_t display_sem;
        std::vector<DisplayUpdate*> display_updates;
        suite_e suite;
        uint32_t* power_idx;
    };

#ifdef __cplusplus
}
#endif





#endif /* DISPLAY_TASK_HPP_ */
