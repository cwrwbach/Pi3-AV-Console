#pragma once 


void clear_screen(uint16_t pixval);


short rgb565(short red,short green,short blue);



void set_pixel(uint16_t * buf,int x, int y, uint16_t colour);

void clear_screen(uint16_t pixval);

short rgb565(short red,short green,short blue);

void plot_line (uint16_t * buf,int x0, int y0, int x1, int y1,uint16_t colour);

void plot_dotted_line (int16_t * buf, int x0, int y0, int x1, int y1,uint16_t colour);

void plot_thick_line (int16_t * buf, int x0, int y0, int x1, int y1,uint16_t colour);


void plot_rectangle(int16_t * buf, int x0, int y0,int sz_x, int sz_y, uint16_t colour);

void plot_thick_rectangle(int16_t * buf, int x0, int y0,int sz_x, int sz_y,uint16_t colour);

/*
void plot_button(int16_t * buf,int x0,int y0,int sz_x, int sz_y,uint16_t backround_col,uint16_t border_col,uint16_t text_col,char text[40])
*/


void plot_large_character(int16_t * buf,int x, int y,uint8_t char_num,uint16_t colour);

void plot_large_string(int16_t * buf , int x, int y,uint8_t * string ,uint16_t colour);


