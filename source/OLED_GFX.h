#ifndef OLED_GFX_h
#define OLED_GFX_h

#include "OLED_1306.h"
#include "I2C_aux.hpp"

class OLED_GFX {
    
  public:
      
    OLED_GFX(OLED_1306 *oled);
    void init();
    void draw_bar(float value, unsigned int max_value, unsigned int row);
    void draw_scaled_bar(float value, unsigned int row);
    void clear_graph(void);  
    OLED_1306 *oled;
  private:
  
    display_tools_t* display_tools;
    unsigned int scale;
     
};
#endif
