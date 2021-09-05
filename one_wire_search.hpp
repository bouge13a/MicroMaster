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
#include "one_wire_command.hpp"

typedef enum {
    ONE_WIRE_SEARCH_ROM,
    ONE_WIRE_ALARM_SEARCH,
}one_wire_search_types_e;

typedef enum {
    OW_SEARCH_IDLE,
    OW_SEARCH_SEND_CMD,
    OW_SEARCH_READ_BIT,
    OW_SEARCH_READ_COMP,
    OW_SEARCH_WRITE_DIR,
    OW_SEARCH_FINISH,
}one_wire_search_states_e;

#ifdef __cplusplus
extern "C" {
#endif

    class OneWireSearch : public ConsolePage {
    public:
        OneWireSearch(GpoObj* gpo_obj,
                      OneWireCmd* one_wire_cmd);
    private :
        void task(OneWireSearch* this_ptr);
        static void taskfunwrapper(void* parm);

        unsigned char crc_check(unsigned char value);

        void set_timer(uint32_t useconds);

        OneWireCmd* one_wire_cmd;

        uint32_t search_type;
        one_wire_search_states_e ow_search_state;
        one_wire_write_states_e one_wire_write_state;
        bool error_flag;
        bool search_result;

        GpoObj* gpo_obj;
        gpio_pin_t* one_wire_pin;

        uint32_t bit_counter;

        uint64_t rom_ids[10];
        int32_t rom_id_idx;

        uint32_t bit_id;
        uint32_t cmp_bit_id;

        bool last_device_flag;
        uint32_t id_bit_number;
        uint32_t last_descrepancy;
        uint32_t last_family_discrepancy;
        uint32_t last_zero;
        uint64_t search_direction;

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
