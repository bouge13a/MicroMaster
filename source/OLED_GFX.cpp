#include "OLED_GFX.h"
#include <cstdlib>

static long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

OLED_GFX::OLED_GFX(OLED_1306 *oled) {
  this->oled = oled; 
  this->display_tools = display_tools;
}

void OLED_GFX::init() {
  // TODO: research this init procedure, we maybe not needing to reset, display_on, and clear, or just reset?
  oled->init_OLED();
  oled->reset_display();
  //oled->display_on();
  //oled->clear_display(); 
}

void OLED_GFX::clear_graph(void) {  
  unsigned char i,k;
  for(k=0;k<7;k++) {	
    oled->set_xy(k,0);    
    for(i=0;i<128;i++) {  
      oled->send_char(0); 
    }
  }
}

void OLED_GFX::draw_bar(float value, unsigned int max_value, unsigned int row) {
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x00);
  oled->send_command(0x10);
  oled->send_char(0x3C);
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x01);
  oled->send_command(0x10);
  oled->send_char(0x42);  
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x02);
  oled->send_command(0x10);   
  
  int scaled = map(value, 0, max_value, 0, 124);

  for(int i=0; i<124; i++) {
    if(i<scaled) {
      oled->send_char(0x5A);
    } else {
      oled->send_char(0x42);
    }
  }
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x0E);
  oled->send_command(0x1F);
  oled->send_char(0x42);   
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x0F);
  oled->send_command(0x1F);
  oled->send_char(0x3C);  
}

void OLED_GFX::draw_scaled_bar(float value, unsigned int row) {
  
  if(value>2000) {    
    scale = 4000;
    oled->send_str_xy("4000", row, 12);
  } else if(value>1000) {   
    scale = 2000;
    oled->send_str_xy("2000", row, 12);
  } else if(value>500) {
    scale = 1000;
    oled->send_str_xy("1000", row, 12);
  } else if(value>100) {
    scale = 500;
    oled->send_str_xy(" 500", row, 12);
  } else if(value<=100) {
    scale = 100;
    oled->send_str_xy(" 100", row, 12);
  }  
      
  oled->send_command(0xb0+row); 
  oled->send_command(0x00);
  oled->send_command(0x10);
  oled->send_char(0x3C);
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x01);
  oled->send_command(0x10);
  oled->send_char(0x42);  
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x02);
  oled->send_command(0x10);   
  
  int scaled = map(value, 0, scale, 0, 92);

  for(int i=0; i<92; i++) {
    if(i<scaled) {
      oled->send_char(0x5A);
    } else {
      oled->send_char(0x42);
    }
  }
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x0E);
  oled->send_command(0x1D);
  oled->send_char(0x42);   
  
  oled->send_command(0xb0+row); 
  oled->send_command(0x0F);
  oled->send_command(0x1D);
  oled->send_char(0x3C);  
  
  
}

