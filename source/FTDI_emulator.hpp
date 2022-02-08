/*
 * FTDI_emulator.hpp
 *
 *  Created on: Oct 5, 2021
 *      Author: steph
 */

#ifndef FTDI_EMULATOR_HPP_
#define FTDI_EMULATOR_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#ifdef __cplusplus
extern "C" {
#endif

    class FtdiEmulator {
    public:
        FtdiEmulator(void);
    private :
        void task(FtdiEmulator* this_ptr);
        static void taskfunwrapper(void* parm);

    };

#ifdef __cplusplus
}
#endif





#endif /* FTDI_EMULATOR_HPP_ */
