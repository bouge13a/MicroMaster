/*
 * I2C_sniffer.hpp
 *
 *  Created on: Sep 7, 2021
 *      Author: steph
 */

#ifndef I2C_SNIFFER_HPP_
#define I2C_SNIFFER_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"
#include "GPOs.hpp"




#ifdef __cplusplus
extern "C" {
#endif

    class I2cSniffer : public ConsolePage {
    public:
        I2cSniffer(GpoObj* gpo_obj);
    private :
        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif



#endif /* I2C_SNIFFER_HPP_ */
