/*
 * one_wire_search.hpp
 *
 *  Created on: Aug 30, 2021
 *      Author: steph
 */

#ifndef ONE_WIRE_SEARCH_HPP_
#define ONE_WIRE_SEARCH_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "console_task.hpp"
#include "GPOs.hpp"



#ifdef __cplusplus
extern "C" {
#endif

    class OneWireSearch : public ConsolePage {
    public:
        OneWireSearch(GpoObj* gpo_obj);
    private :
        void task(OneWireSearch* this_ptr);
        static void taskfunwrapper(void* parm);

        GpoObj* gpo_obj;

        QueueHandle_t one_wire_q;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* ONE_WIRE_SEARCH_HPP_ */
