/*
 * I2C_sniffer.hpp
 *
 *  Created on: Sep 14, 2021
 *      Author: steph
 */

#ifndef I2C_SNIFFER_HPP_
#define I2C_SNIFFER_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"
#include "GPIs.hpp"


typedef enum {
    I2CS_STOP_CONDITION,
    I2CS_GET_DATA,
    I2CS_DETERMINE_CONDITION,
    I2CS_DETERMINE_STOP,
    I2CS_REPEATED_START,
}i2c_sniff_states_e;

    class I2cSniffer : public ConsolePage {
    public:
        I2cSniffer(GpiObj* gpo_obj);
    private :

        void task(I2cSniffer* this_ptr);
        static void taskfunwrapper(void* parm);

        uint32_t rx_char;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };






#endif /* I2C_SNIFFER_HPP_ */
