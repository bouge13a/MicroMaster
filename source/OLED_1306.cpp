#include "OLED_1306.h"
#include "OLED_data.h"
#include "FreeRTOS.h"

OLED_1306::OLED_1306(display_tools_t* display_tools) {
    this->display_tools = display_tools;
    this->display_update_sem = xSemaphoreCreateBinary();
}

void OLED_1306::draw_graph(unsigned int x, unsigned int y) {
  draw_pixel(x+9, (y*2)+8);
  draw_pixel(x+9, (y*2)+9);  
}

void OLED_1306::draw_pixel(unsigned int x, unsigned int y) {
  send_command(0xb0 + ((63-x)/8));  
  send_command(0x00+(y&0x0f));
  send_command(0x10+((y>>4)&0x0f));  
  send_char(0x01<<((63-x)%8));      
}

void OLED_1306::send_char_xy(unsigned char data, int X, int Y) {  
  set_xy(X, Y);
  I2cMsgAux i2c_msg;
  uint8_t tx_data[9];
  tx_data[0] = 0x40u;
  for(int i=1;i<9;i++) {
      send_char(myFont[data-0x20][i]);
  }    
  i2c_msg.address = OLED_address;
  i2c_msg.tx_data = tx_data;
  i2c_msg.num_tx_bytes = 9;
  i2c_msg.num_rx_bytes = 0;
  i2c_msg.semphr = this->display_update_sem;
  this->display_tools->i2c->add_i2c_msg(&i2c_msg);
  xSemaphoreTake(this->display_update_sem, portMAX_DELAY);
}

void OLED_1306::send_str(unsigned char *string) {
  unsigned char i=0;
  while(*string) {
    for(i=0;i<8;i++) {
        send_char(myFont[*string-0x20][i]);
    }
    *string++;
  }
}

void OLED_1306::send_str_xy(char *string, int X, int Y) {
  set_xy(X,Y);
  unsigned char i=0;
  while(*string) {
    for(i=0;i<8;i++) {
      send_char(myFont[*string-0x20][i]);
    }
    *string++;
  }
}

void OLED_1306::send_char(unsigned char data) { 

    I2cMsgAux i2c_msg;
    uint8_t tx_data[] = {0x40, data};
    i2c_msg.address = OLED_address;
    i2c_msg.tx_data = tx_data;
    i2c_msg.num_tx_bytes = 2;
    i2c_msg.num_rx_bytes = 0;
    i2c_msg.semphr = this->display_update_sem;
    this->display_tools->i2c->add_i2c_msg(&i2c_msg);
    xSemaphoreTake(this->display_update_sem, portMAX_DELAY);
}

void OLED_1306::send_command(unsigned char command) {

  I2cMsgAux i2c_msg;
  uint8_t tx_data[] = {0x80, command};
  i2c_msg.address = OLED_address;
  i2c_msg.tx_data = tx_data;
  i2c_msg.num_tx_bytes = 2;
  i2c_msg.num_rx_bytes = 0;
  i2c_msg.semphr = this->display_update_sem;
  this->display_tools->i2c->add_i2c_msg(&i2c_msg);
  xSemaphoreTake(this->display_update_sem, portMAX_DELAY);

}

void OLED_1306::set_xy(unsigned char row,unsigned char col) {
  send_command(0xb0+row);
  send_command(0x00+(8*col&0x0f));
  send_command(0x10+((8*col>>4)&0x0f));
}

void OLED_1306::reset_display(void) {  
  display_off();
  clear_display();  
  display_on();  
}

void OLED_1306::display_on(void) {
  send_command(0xaf);   
}

void OLED_1306::display_off(void) {
  send_command(0xae);
}

void OLED_1306::clear_display(void) {  
  unsigned char i,k;
  for(k=0;k<8;k++) {	
    set_xy(k,0);    
    for(i=0;i<130;i++) {
      send_char(0); 
    }
  }
}

void OLED_1306::init_OLED(void) {
  
  send_command(0xae);	
  send_command(0xa6);
  
  // Adafruit Init sequence for 128x64 OLED module
  
  send_command(0xAE);            // DISPLAYOFF
  send_command(0xD5);            // SETDISPLAYCLOCKDIV
  send_command(0x80);            // the suggested ratio 0x80
  send_command(0xA8);            // SSD1306_SETMULTIPLEX
  send_command(0x3F);
  send_command(0xD3);            // SETDISPLAYOFFSET
  send_command(0x0);             // no offset
  send_command(0x40 | 0x0);      // SETSTARTLINE
  send_command(0x8D);            // CHARGEPUMP
  send_command(0x14);
  send_command(0x20);            // MEMORYMODE
  send_command(0x00);            // 0x0 act like ks0108
    
  send_command(0xA0 | 0x1);      // SEGREMAP   // Rotate screen 180 deg
  //send_command(0xA0);
    
  send_command(0xC8);            // COMSCANDEC // Rotate screen 180 Deg
  //send_command(0xC0);
    
  send_command(0xDA);            // 0xDA
  send_command(0x12);            // COMSCANDEC
  send_command(0x81);            // SETCONTRAS
  send_command(0x80);            //
  send_command(0xd9);            // SETPRECHARGE 
  send_command(0x22);
  send_command(0xDB);            // SETVCOMDETECT                
  send_command(0x20);
  send_command(0xA4);            // DISPLAYALLON_RESUME        
  send_command(0xA6);            // NORMALDISPLAY             

  clear_display();
  send_command(0x2e);            // stop scroll m,8

  //----------------------------REVERSE comments----------------------------//
  //  send_command(0xa0);		// seg re-map 0->127(default)
  //  send_command(0xa1);		// seg re-map 127->0
  //  send_command(0xc8);
  //  delay(1000);
  //----------------------------REVERSE comments----------------------------//  
  // send_command(0xa7);                // Set Inverse Display  
  // send_command(0xae);		// display off
  send_command(0x20);                   // Set Memory Addressing Mode
  send_command(0x00);                   // Set Memory Addressing Mode ab Horizontal addressing mode
  // send_command(0x02);                // Set Memory Addressing Mode ab Page addressing mode(RESET)  
  

  
  /*
  for(int i=0;i<128*8;i++) {
    send_char(pgm_read_byte(logo+i));
  }
  */
  
  send_command(0xaf);                    // display on
  set_xy(0,0);
   
}
