#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>

#include "avc-lib.h"
#include "avc-colours.h"

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int fbfd; // framebuffer file descriptor
short * frame_buf;
long int screensize ;

/*
#define TITLE_PANEL_TOP 0
#define TITLE_PANEL_BOT 49
#define GRAPH_PANEL_TOP 50
#define GRAPH_PANEL_BOT 249
#define APP_PANEL_TOP 250
#define APP_PANEL_BOT 349
#define SYS_PANEL_TOP 550
#define SYS_PANEL_BOT 599
*/

#define STATUS_POS 0

#define STATUS_Y 50
#define SYSTEM_Y 100

int screen_size_x;
int screen_size_y;
int status_pos;
int system_pos;
int graph_pos;
int control_pos;

int graph_y;
int control_y;

uint16_t * status_buf;
uint16_t * system_buf;
uint16_t * graph_buf;
uint16_t * control_buf;

//================



void main()
{
unsigned int red,green,blue;
short rgba;
int screenbytes;
int quit_request;
int err;


fbfd = open("/dev/fb0", O_RDWR); // Open the framebuffer device file for reading and writing
if (fbfd == -1) 
    printf("Error: cannot open framebuffer device.\n");
 
if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) // Get fixed screen information
    printf("Error reading fixed information.\n");

if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) // Get variable screen information
	    printf("Error reading variable screen info.\n");
printf("Display info %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

screen_size_x =vinfo.xres;
screen_size_y = vinfo.yres;
screenbytes = finfo.smem_len;

// map framebuffer to user memory 
frame_buf = (short*)mmap(0, screenbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
close(fbfd); //It seems we can close this straight away (see Man)

status_pos = STATUS_POS;
system_pos = screen_size_y - SYSTEM_Y;

graph_y = 300;
graph_pos = 50;
control_y = 300;
control_pos = 350;

status_buf = malloc(STATUS_Y*screen_size_x*2);
system_buf = malloc(SYSTEM_Y*screen_size_x*2);
graph_buf =  malloc(graph_y*screen_size_x*2);
control_buf = malloc(control_y*screen_size_x*2);

clear_screen(rgb565(0x0,0x3,0x00));

sleep(2);
short pixy;

//test the title buffer
pixy = YELLOW;
for(int p=0;p<(screen_size_x * STATUS_Y);p++)
    status_buf[p] = pixy;
plot_large_string(status_buf,400,10,"STATUS",BLACK);

//refresh graph_buf
memcpy(frame_buf+(status_pos * screen_size_x),status_buf,screen_size_x * STATUS_Y*2);

sleep(4);

//test the graph buffer
pixy = C_DARK_BLUE;

for(int p=0;p<(screen_size_x* graph_y );p++)
    graph_buf[p] = pixy;

plot_large_string(graph_buf,400,10,"GRAPH",BLACK);
//plot_line(graph_buf, 20,20,300,62,WHITE);


//refreash graph
memcpy(frame_buf+(graph_pos * screen_size_x),graph_buf,screen_size_x * graph_y *2);

//test app buffer
sleep(4);

//---
pixy = MAGENTA;

for(int p=0;p<(screen_size_x* control_y );p++)
    control_buf[p] = pixy;
plot_large_string(control_buf,400,10,"CONTROL",BLACK);
//plot_thick_line(graph_buf,100,100,600,100,WHITE);

memcpy(frame_buf+(control_pos * screen_size_x),control_buf,screen_size_x * control_y *2);


sleep(4);


pixy = GREEN;

for(int p=0;p<(screen_size_x* SYSTEM_Y);p++)
    system_buf[p] = pixy;

plot_large_string(system_buf,400,10,"SYSTEM",BLACK);

memcpy(frame_buf+(system_pos * screen_size_x),system_buf,screen_size_x * SYSTEM_Y *2);





return;

//plot_thick_line(graph_buf,100,100,600,100,WHITE);

//plot_rectangle(frame_buf,200,150,450,25,GREEN);


//for(int b=0;b<10;b++)
plot_filled_rectangle(frame_buf,200+3,150+4,450-5,50-4,C_DARK_VIOLET);


plot_thick_rectangle(frame_buf,200,150,450,50,WHITE);

memcpy(frame_buf+(screen_size_x * 250),graph_buf,screen_size_x * 300 *2);

//test sys buffer





//for(int n=0;n<300;n++)
  //  set_screen_pixel( 100+n,200+n,YELLOW) ; //rgb565(0x0f,0x0f,0x0f));

plot_line(frame_buf,100,100,200,130,YELLOW);

plot_thick_line(frame_buf,100,100,600,100,YELLOW);



plot_large_character(frame_buf,300,750,'A',WHITE);

for(int b=1;b<10;b++)
    plot_filled_rectangle(frame_buf,20+ ( b * 120),730,100,30,C_DARK_VIOLET);

plot_large_string(frame_buf,400,733,"Marvellous !",CYAN);
//plot_large_character(graph_buf,50,100,'B',WHITE);

//memcpy(frame_buf+(screen_size_x * 50),graph_buf,screen_size_x * 300 *2);


while(1) sleep(1); //stopper
}

