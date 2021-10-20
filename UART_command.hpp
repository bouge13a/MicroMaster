/*
 * UART_command.hpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */

#ifndef UART_COMMAND_HPP_
#define UART_COMMAND_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"

typedef enum {
    UART_GET_SPEED,
    UART_GET_FORMAT,
    UART_GET_STRING,
    UART_GET_HEX,
}uart_cmd_state_e;



void uart_set_parity(uint32_t index);
void uart_set_stop_bits(uint32_t index);

class UartCmd : public ConsolePage {
public:
    UartCmd(void);
    void send_message(void);
    uint32_t get_speed(void);
private :


    char* char_string;
    uint32_t char_string_index;
    uint32_t last_char_string_index;
    uart_cmd_state_e cmd_state;
    uart_cmd_state_e msg_type;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_help(void);
    void draw_reset(void);
};





#endif /* UART_COMMAND_HPP_ */
