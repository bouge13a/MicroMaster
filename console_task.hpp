/*
 * console_gui.hpp
 *
 *  Created on: Jul 27, 2020
 *      Author: steph
 */

#ifndef CONSOLE_TASK_HPP_
#define CONSOLE_TASK_HPP_

#include <stdint.h>
#include <vector>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#ifdef __cplusplus
extern "C" {
#endif

    namespace ArrowKeys {
        enum arrow_keys {
            UP = 21,
            DOWN,
            RIGHT,
            LEFT,
            ESCAPE = 27,
        };
    }

    class ConsolePage {
    public:
        ConsolePage(const char* name,
                    const uint32_t refresh_rate,
                    const bool write_to_page);

        // Page draw functions
        virtual void draw_page(void)           = 0;
        virtual void draw_data(void)           = 0;
        virtual void draw_input(int character) = 0;
        virtual void draw_help(void)           = 0;
        virtual void draw_reset(void)          = 0;

        const char* name;
        const uint32_t refresh_rate;
        bool on_screen;
        bool write_to_page;

    };


    class ConsoleTask : public ConsolePage {
    public:
        ConsoleTask(QueueHandle_t uart_rx_queue);

        void add_page(ConsolePage* page);

    private:
        static void taskfunwrapper(void* parm);
        void task(ConsoleTask* this_ptr);
        TaskHandle_t task_handle;

        void start_draw_menu(ConsoleTask* this_ptr);

        QueueHandle_t uart_rx_q;

        uint32_t page_index;
        uint32_t last_page_index;

        std::vector<ConsolePage*> pages;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);

    };

#ifdef __cplusplus
}
#endif


#endif /* CONSOLE_TASK_HPP_ */
