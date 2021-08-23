/*
 * SPI_monitor.hpp
 *
 *  Created on: Aug 23, 2021
 *      Author: steph
 */

#ifndef SPI_MONITOR_HPP_
#define SPI_MONITOR_HPP_



#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <vector>


#include "console_task.hpp"
#include "SPI_command.hpp"

typedef enum {
    SPI_HEX_FORMAT,
    SPI_BIN_FORMAT,
    SPI_MSB_DEC_FORMAT,
    SPI_LSB_DEC_FORMAT,
    SPI_NUM_OF_FORMATS,
}spi_msg_format_e;

#ifdef __cplusplus
extern "C" {
#endif

    class SpiMonitorTask : public ConsolePage {
    public:
        SpiMonitorTask(SpiCmdTask* spi, std::vector<SpiMsg*>& spi_monitor_msgs);
    private :
        void task(SpiMonitorTask* this_ptr);
        static void taskfunwrapper(void* parm);
        TaskHandle_t task_handle;
        uint32_t test;

        SpiCmdTask* spi;
        std::vector<SpiMsg*> spi_monitor_msgs;
        spi_msg_format_e* message_format;
        uint32_t format_index;

        void print_format_row(uint32_t row);
        bool print_data(uint32_t inner_index, uint32_t index);

        void draw_page(void);
        void draw_data(void);
        void draw_input(int character);
        void draw_help(void);
        void draw_reset(void);
    };

#ifdef __cplusplus
}
#endif


#endif /* SPI_MONITOR_HPP_ */
