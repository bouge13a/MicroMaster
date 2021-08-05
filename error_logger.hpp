/*
 * error_logger.hpp
 *
 *  Created on: Jul 29, 2021
 *      Author: steph
 */

#ifndef ERROR_LOGGER_HPP_
#define ERROR_LOGGER_HPP_

#include "console_task.hpp"

typedef struct {
    const char* name;
    const char* info;
    uint32_t occurences;
}error_t;

class ErrorLogger : public ConsolePage {
public :
   static ErrorLogger* get_instance(void);

   error_t* create_error(const char* name,
                         const char* info);

   void set_error(error_t* error);

private:
    ErrorLogger(void);
    static ErrorLogger* obj;

    void draw_page(void);
    void draw_data(void);
    void draw_input(int character);
    void draw_help(void);
    void draw_reset(void);
};


#endif /* ERROR_LOGGER_HPP_ */
