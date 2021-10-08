/*
 * neopixel_control.hpp
 *
 *  Created on: Oct 8, 2021
 *      Author: steph
 */

#ifndef NEOPIXEL_CONTROL_HPP_
#define NEOPIXEL_CONTROL_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "console_task.hpp"
#include "GPOs.hpp"

typedef enum {
    NEOPIX_IDLE,
}neopix_states_e;

class NeopixMsg {
public:
    NeopixMsg(void);

};

#ifdef __cplusplus
extern "C" {
#endif

    class NeopixelCtl : public ConsolePage {
    public:
        NeopixelCtl(GpoObj* gpo_object);
    private :
        void task(NeopixelCtl* this_ptr);
        static void taskfunwrapper(void* parm);

        QueueHandle_t neopix_msg_q;
        neopix_states_e neopix_state;
        NeopixMsg* neopix_msg;
        GpoObj* gpo_object;
        gpio_pin_t* data_pin;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* NEOPIXEL_CONTROL_HPP_ */
