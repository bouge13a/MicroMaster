/*
 * console_task.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */
#include "console_task.hpp"
#include "text_controls.hpp"

#include <assert.h>
#include <UART_to_USB.hpp>
#include "uartstdio.h"

// Width of the Menu bar
static const uint32_t MENU_BAR_WIDTH = 70;

ConsolePage::ConsolePage(const char* name,
                         uint32_t refresh_rate,
                         const bool write_to_page)
                         : name(name),
                           refresh_rate(refresh_rate),
                           write_to_page(write_to_page) {

    this->on_screen = false;

} // End ConsolePage::ConsolePage

ConsoleTask::ConsoleTask (QueueHandle_t uart_rx_queue) : ConsolePage ("Home Page",
                                                                      portMAX_DELAY,
                                                                      false) {

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Console",                                    /* Text name for the task. */
                120,                                          /* Stack size in words, not bytes. */
                this,                                         /* Parameter passed into the task. */
                3,                                            /* Priority at which the task is created. */
                &this->task_handle );                         /* Used to pass out the created task's handle. */

    assert(this->task_handle);
    assert(uart_rx_queue);

    this->uart_rx_q = uart_rx_queue;
    this->page_index = 0;
    this->last_page_index = 0;

    this->add_page(this);

} // End ConsoleTask::ConsoleTask

void ConsoleTask::add_page(ConsolePage* page) {
    assert(page);

    this->pages.push_back(page);

} // End ConsoleTask::add_page

void ConsoleTask::taskfunwrapper(void* parm){
    (static_cast<ConsoleTask*>(parm))->task((ConsoleTask*)parm);
} // End UartTask::taskfunwrapper

void ConsoleTask::start_draw_menu(ConsoleTask* this_ptr) {

    // loop index
    uint32_t idx;

    TextCtl::clear_terminal();

    // Place the cursor in the middle of the menu bar
    // minus half the number of console pages
    TextCtl::cursor_pos(0, MENU_BAR_WIDTH/2);

    //draw the rest of the asterisks
    for(idx=0; idx<this_ptr->pages.size(); idx++){
        if (idx == this_ptr->page_index) {

            // Reverse the backround color and print an *
            TextCtl::set_text_mode(TextCtl::mode_reverse);
            UARTprintf("*");
            TextCtl::set_text_mode(TextCtl::mode_reset);
        } else {

            UARTprintf("*");
        }
    }

    // Set the cursor position to the next line and print name
    TextCtl::cursor_pos(2, MENU_BAR_WIDTH/2);
    UARTprintf("%s", this_ptr->pages[this_ptr->page_index]->name);

    // set the cursor position to next line and print menu bar
    TextCtl::cursor_pos(3, 0);
    for(idx=0; idx<MENU_BAR_WIDTH; idx++) {
        UARTprintf("-");
    }

    TextCtl::cursor_pos(4,0);


} // End draw_menu

void ConsoleTask::task(ConsoleTask* this_ptr) {

    uint8_t rx_char = 0;

    xQueueReceive(this_ptr->uart_rx_q, &rx_char, portMAX_DELAY);

    TextCtl::clear_terminal();

    TextCtl::set_text_mode(TextCtl::mode_concealed);

    this_ptr->start_draw_menu(this_ptr);

    this_ptr->pages[0]->draw_page();

    while(1) {
        rx_char = 0;

        xQueueReceive(this_ptr->uart_rx_q, &rx_char, this_ptr->pages[this_ptr->page_index]->refresh_rate);

        // 9 = TAB
        if (9 == rx_char) {

            this_ptr->pages[this_ptr->last_page_index]->on_screen = false;
            this_ptr->pages[this_ptr->last_page_index]->draw_reset();
            this_ptr->last_page_index = this_ptr->page_index;
            this_ptr->page_index = (this_ptr->page_index + 1) % this_ptr->pages.size();
            this_ptr->pages[this_ptr->page_index]->on_screen = true;
            this_ptr->start_draw_menu(this_ptr);
            this_ptr->last_page_index = this_ptr->page_index;
            this_ptr->pages[this_ptr->page_index]->draw_page();
            TextCtl::set_text_mode(TextCtl::mode_concealed);

        } else if(this_ptr->page_index != 0 && rx_char == 'r' && !this_ptr->pages[page_index]->write_to_page) {

            this_ptr->pages[page_index]->draw_reset();

        } else if(this_ptr->page_index != 0 && rx_char == 'h' && !this_ptr->pages[page_index]->write_to_page) {

            this_ptr->start_draw_menu(this_ptr);
            TextCtl::cursor_pos(5, 0);
            UARTprintf("Press ESC to return to homepage\r\n\n");
            this_ptr->pages[page_index]->on_screen = false;
            this_ptr->pages[page_index]->draw_help();


        } else if ((this_ptr->page_index == 0) && (rx_char >= 'a' && (((int32_t)this_ptr->pages.size() - (int32_t)(rx_char-'a') >=0 )))) {

           this_ptr->pages[page_index]->on_screen = false;
           this_ptr->page_index = rx_char - 'a';
           this_ptr->pages[page_index]->on_screen = true;
           this_ptr->last_page_index = this->page_index;
           this_ptr->start_draw_menu(this_ptr);
           this_ptr->pages[rx_char-'a']->draw_page();

        }  else if (ArrowKeys::ESCAPE == rx_char) {

            if ('[' == get_char()) {
                // Control sequence detected
                // Perform the control sequence
                switch(get_char()) {
                case 'A':
                    this_ptr->pages[this_ptr->page_index]->draw_input(ArrowKeys::UP);
                    break;

                case 'B':
                    this_ptr->pages[this_ptr->page_index]->draw_input(ArrowKeys::DOWN);
                    break;

                case 'C':
                    this_ptr->pages[this_ptr->page_index]->draw_input(ArrowKeys::RIGHT);
                    break;

                case 'D':
                    this_ptr->pages[this_ptr->page_index]->draw_input(ArrowKeys::LEFT);
                    break;

                default:
                    break;

                }

            } else {

                // Escape key was pressed
                this_ptr->pages[page_index]->on_screen = false;
                this_ptr->page_index = 0;
                this_ptr->start_draw_menu(this_ptr);
                this_ptr->pages[page_index]->on_screen = true;
                this_ptr->last_page_index = 0;
                this_ptr->pages[0]->draw_page();

            }
        } else {
            this_ptr->pages[this_ptr->page_index]->draw_input(rx_char);
        }

        if (0 == rx_char && this_ptr->pages[this_ptr->page_index]->on_screen) {
            this_ptr->pages[this_ptr->page_index]->draw_data();
        }


        TextCtl::set_text_mode(TextCtl::mode_concealed);


    }

} // End ConsoleTask::task

void ConsoleTask::draw_page(void) {
   uint32_t index = 0;
   char letter = 'a';

   for(index=0; index < this->pages.size(); index++) {
       UARTprintf("%c : %s\n\r", letter++, this->pages[index]->name );
   }
}

void ConsoleTask::draw_data(void) {

}

void ConsoleTask::draw_input(int character) {

}

void ConsoleTask::draw_help(void) {

}

void ConsoleTask::draw_reset(void) {


}


