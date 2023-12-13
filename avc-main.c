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


#define TITLE_PANEL_TOP 0
#define TITLE_PANEL_BOT 49
#define GRAPH_PANEL_TOP 50
#define GRAPH_PANEL_BOT 249
#define APP_PANEL_TOP 250
#define APP_PANEL_BOT 349
#define SYS_PANEL_TOP 550
#define SYS_PANEL_BOT 599

uint16_t title_buf[TITLE_PANEL_TOP * TITLE_PANEL_BOT];
uint16_t graph_buf[GRAPH_PANEL_TOP * GRAPH_PANEL_BOT];
uint16_t app_buf[APP_PANEL_TOP * APP_PANEL_BOT];
uint16_t sys_buf[SYS_PANEL_TOP * SYS_PANEL_BOT];

int screen_size_x;
int screen_size_y;

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

//screensize=screenbytes/vinfo.bits_per_pixel/8; //2 bytes per pixel
//printf(" \n screensize=%d shorts\n",screensize);


// map framebuffer to user memory 
frame_buf = (short*)mmap(0, screenbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
close(fbfd); //It seems we can close this straight away (see Man)

clear_screen(rgb565(0x0,0x0,0x03));

usleep(200000);
short pixy;

//test the title buffer
pixy = RED;
for(int p=0;p<(screen_size_x* 25 );p++)
    graph_buf[p] = pixy;
memcpy(frame_buf+(0),graph_buf,screen_size_x * 25*2);

//test the graph buffer
pixy = DARK_GREEN;

for(int p=0;p<(screen_size_x* 300 );p++)
    graph_buf[p] = pixy;

plot_line(graph_buf, 20,20,300,62,WHITE);



memcpy(frame_buf+(screen_size_x * 50),graph_buf,screen_size_x * 300 *2);

//test app buffer

pixy = MAGENTA;

for(int p=0;p<(screen_size_x* 300 );p++)
    graph_buf[p] = pixy;

plot_thick_line(graph_buf,100,100,600,100,WHITE);

plot_rectangle(frame_buf,200,150,450,25,GREEN);

plot_thick_rectangle(frame_buf,700,150,50,25,CYAN);

memcpy(frame_buf+(screen_size_x * 250),graph_buf,screen_size_x * 300 *2);

//test sys buffer





//for(int n=0;n<300;n++)
  //  set_screen_pixel( 100+n,200+n,YELLOW) ; //rgb565(0x0f,0x0f,0x0f));

plot_line(frame_buf,100,100,200,130,YELLOW);

plot_thick_line(frame_buf,100,100,600,100,YELLOW);



plot_large_character(frame_buf,300,750,'A',WHITE);

plot_large_string(frame_buf,400,750,"Marvellous !",CYAN);
//plot_large_character(graph_buf,50,100,'B',WHITE);

//memcpy(frame_buf+(screen_size_x * 50),graph_buf,screen_size_x * 300 *2);


while(1) sleep(1); //stopper
}

