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
#include "GPOs.hpp"

#include "SPI_command.hpp"


typedef enum {

};

#ifdef __cplusplus
extern "C" {
#endif

    class SpiSniffer : public ConsolePage {
    public:
        SpiSniffer(SpiCmdTask* spi_cmd);
    private :
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
