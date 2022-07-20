#ifndef OLED_1306_h
#define OLED_1306_h

#define OLED_address  0x3c

#include "I2C_aux.hpp"
#include "FreeRTOS.h"
#include "semphr.h"

class OLED_1306 {
  
  public:
  
    OLED_1306(display_tools_t* display_tools);
    
    void init_OLED();
    
    void reset_display(void);    
    void display_on(void);    
    void display_off(void);    
    void clear_display(void);        
       
    void send_str(unsigned char *string);    
    void send_str_xy(char *string, int X, int Y);
    void send_char_xy(unsigned char data, int X, int Y); 
    
    void set_xy(unsigned char row,unsigned char col);   
    void draw_bar(float value, unsigned int max_value, unsigned int row);
    void draw_pixel(unsigned int x, unsigned int y);
    void draw_graph(unsigned int x, unsigned int y);    
    
    // TODO: protected?
    void send_command(unsigned char command);
    void send_char(unsigned char data);  
       
  private:
  
    display_tools_t* display_tools;
    SemaphoreHandle_t display_update_sem;

 
};
#endif
