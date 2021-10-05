/*
 * console_task.cpp
 *
 *  Created on: Jul 26, 2021
 *      Author: steph
 */
#include "console_task.hpp"
#include "text_controls.hpp"
#include "initialization.hpp"

#include <assert.h>
#include <UART_to_USB.hpp>
#include "uartstdio.h"

// Width of the Menu bar
static const uint32_t MENU_BAR_WIDTH = 78;
static const uint32_t SECOND_COL     = 35;
static const uint32_t START_ROW      = 7;

static const uint32_t PS_MENU_SPACING = 15;
static const uint32_t PS_MENU_COL  = 25;
static const uint32_t PS_MENU_ROW  = 20;


static const uint32_t PS_NUM = 3;
static const char* ps_menu[] = {
                              "3v3",
                              "5v",
                              "2v8",
};

ConsolePage::ConsolePage(const char* name,
                         uint32_t refresh_rate,
                         const bool write_to_page)
                         : name(name),
                           refresh_rate(refresh_rate),
                           write_to_page(write_to_page) {

    this->on_screen = false;

} // End ConsolePage::ConsolePage

ConsoleTask::ConsoleTask (QueueHandle_t uart_rx_queue,
                          uint32_t* power_idx) : ConsolePage ("Home Page",
                                                              portMAX_DELAY,
                                                              false) {

    this->pages.reserve(26);
    this->power_idx = power_idx;

    xTaskCreate(this->taskfunwrapper, /* Function that implements the task. */
                "Console",                                    /* Text name for the task. */
                200,                                          /* Stack size in words, not bytes. */
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

void ConsoleTask::draw_ps_menu(uint32_t index) {

    TextCtl::text_color(TextCtl::black_text);

    for(uint32_t loop_idx=0; loop_idx<PS_NUM; loop_idx++) {

        TextCtl::bgd_color(TextCtl::white_bgd);

        if(loop_idx == index) {
            TextCtl::bgd_color(TextCtl::blue_bgd);
        }

        TextCtl::cursor_pos(PS_MENU_ROW, PS_MENU_COL + loop_idx*PS_MENU_SPACING);
        UARTprintf("%s", ps_menu[loop_idx]);


    }

    TextCtl::bgd_color(TextCtl::black_bgd);
    TextCtl::text_color(TextCtl::white_text);

} // End ConsoleTask::draw_ps_menu


uint32_t ConsoleTask::draw_start_page(ConsoleTask* this_ptr) {

    uint8_t rx_char=0;
    uint32_t menu_index = 0;

    TextCtl::cursor_pos(10, 30);
    UARTprintf("MicroMaster Mini Version 0");
    TextCtl::cursor_pos(12, 30);
    UARTprintf("Press any key to continue");

    this_ptr->draw_ps_menu(0);

    while(1) {

        xQueueReceive(this_ptr->uart_rx_q, &rx_char, portMAX_DELAY);

        if (ArrowKeys::ESCAPE == rx_char) {

            if ('[' == get_char()) {
                // Control sequence detected
                // Perform the control sequence
                switch(get_char()) {
                case 'C':
                    menu_index = (menu_index+1) % PS_NUM;
                    this_ptr->draw_ps_menu(menu_index);
                    break;

                case 'D':

                    if (menu_index <= 0) {
                        menu_index = PS_NUM - 1;
                    } else {
                        menu_index--;
                    }

                    this_ptr->draw_ps_menu(menu_index);

                    break;

                default:
                    break;

                }

            } else {

                // Escape key was pressed


            }
        } else {
            return menu_index;
        }
    }
}


void ConsoleTask::start_draw_menu(ConsoleTask* this_ptr) {

    // loop index
    uint32_t idx;

    TextCtl::clear_terminal();

    // Place the cursor in the middle of the menu bar
    // minus half the number of console pages
    TextCtl::cursor_pos(0, MENU_BAR_WIDTH/2 - this_ptr->pages.size()/2);

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
    TextCtl::cursor_pos(2, MENU_BAR_WIDTH/2 - this_ptr->pages.size()/2);
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

    *this_ptr->power_idx = this_ptr->draw_start_page(this_ptr);

    new PostScheduler();

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

            xQueueReceive(this_ptr->uart_rx_q, &rx_char, portMAX_DELAY);

            this_ptr->pages[page_index]->draw_reset();
            this_ptr->start_draw_menu(this_ptr);
            this_ptr->pages[this_ptr->page_index]->draw_page();



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
                this_ptr->pages[this_ptr->page_index]->draw_reset();
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

   UARTprintf("Welcome to MicroMaster\r\n");
   UARTprintf("Press a letter to travel to a page, or tab to scroll through pages");

   for(index=0; index < this->pages.size(); index++) {

       if(index > this->pages.size()/2) {
           TextCtl::cursor_pos(START_ROW + index - this->pages.size()/2 - 1, SECOND_COL);
       } else {
           TextCtl::cursor_pos(START_ROW + index, 0);
       }
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


