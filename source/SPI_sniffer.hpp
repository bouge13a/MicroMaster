/*
 * I2C_sniffer.hpp
 *
 *  Created on: Sep 7, 2021
 *      Author: steph
 */

#ifndef SPI_SNIFFER_HPP_
#define SPI_SNIFFER_HPP_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console_task.hpp"

#include "SPI_command.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    class SpiSniffer : public ConsolePage {
    public:
        SpiSniffer(SpiCmdTask* spi_cmd);
    private :
        void tx_task(SpiSniffer* this_ptr);
        static void tx_taskfunwrapper(void* parm);

        void rx_task(SpiSniffer* this_ptr);
        static void rx_taskfunwrapper(void* parm);

        void init_spi_sniffer(void);

        SpiCmdTask* spi_cmd;

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);

    };

#ifdef __cplusplus
}
#endif



#endif /* SPI_SNIFFER_HPP_ */
