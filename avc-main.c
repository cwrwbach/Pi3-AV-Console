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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>


#include "avc-lib.h"
#include "avc-colours.h"

#define FRAME_BUF_WIDTH 800
#define FRAME_BUF_HEIGHT 480
#define SCOPE_WIDTH 700
#define SCOPE_HEIGHT 300

#define WFALL_WIDTH 700
#define WFALL_HEIGHT 300

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int fbfd; // framebuffer file descriptor

long int screensize ;

uint screen_size_x;
uint screen_size_y;
uint status_pos;

uint32_t * frame_buf;
uint32_t * scope_buf;
uint32_t * wfall_buf;

//================


int tiempo(void) {
//int fb = open("/dev/fb0", O_RDWR);
__u32 dummy = 0;

int i;
struct timeval tv;
unsigned long time_us,start,stop,delta;
int error_code;
float rate;

gettimeofday(&tv, NULL);
start = 1000000 * tv.tv_sec + tv.tv_usec;

for (i = 0; i < 100; i++) 
    {
    error_code = ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy);
    gettimeofday(&tv, NULL);
    time_us = 1000000 * tv.tv_sec + tv.tv_usec;
  //  printf("Time is %lu, error code is %i\n", time_us, error_code);
    }

gettimeofday(&tv, NULL);
stop = 1000000 * tv.tv_sec + tv.tv_usec;
delta = stop - start;

printf(" delta: %lu mSecs \n",delta/1000);

rate = (float)100000000 / (float) delta;

printf(" Frame rate = %f \n",rate); 

}



void draw_grid()
{
int i,x,y;
int n_horiz;
int n_verts;
int h_gap,v_gap;

n_horiz=8;
n_verts = 12;

v_gap = SCOPE_WIDTH/(n_verts-2);
h_gap = SCOPE_HEIGHT/(n_horiz);

for(i=1;i<n_horiz-1;i++)
    plot_dotted_line(scope_buf,0,i*h_gap,SCOPE_WIDTH,i*h_gap,0x00808000);//YELLOW);

for(i=1;i<n_verts-2;i++)
    plot_dotted_line(scope_buf,i*v_gap,0,i*v_gap,SCOPE_HEIGHT-50,0x00008000);//);
}

//------------------

void draw_fft()
{
int bins_n;
int val; 
int nv;

nv=0;

bins_n = 600;


for(int n = 0; n<bins_n; n++)
    {
    val = 230 - rand() % 50;
    plot_line(frame_buf, nv+100, val, nv+100, 230, WHITE);
    nv++;
    }
}
//=========



#if 0


void draw_waterfall()
{
uint32_t colour;
int point;
unsigned char fft_val;
int loc_x,loc_y;
unsigned int wf_ln;


loc_x = 10;
loc_y = 10;

wf_ln++;
if(wf_ln > WFALL_HEIGHT)
    wf_ln = 1;


//Draw first line of waterfall
for(point=0;point<600;point++) //FFT SIZE
    {
    fft_val = (rand() % 100) ;//255-(fft_video_buf[point]);
    fft_val*=1 ;
    colour = 0x00204040; // rgb565(turbo[fft_val][0]/8,turbo[fft_val][1]/4,turbo[fft_val][2]/8);

    colour = C_DIM_GRAY;
    if (fft_val > 20) colour = GREEN;
    if (fft_val > 80) colour = RED;    

    set_pixel(wfall_buf,point , 0, colour);
   // set_pixel(wfall_buf,point+1 , 0, colour);
   // set_pixel(wfall_buf,point+2 , 0, colour);
   // set_pixel(wfall_buf,point+3, 0, colour);
   // set_pixel(wfall_buf,point+4 , 0, colour);
   // set_pixel(wfall_buf,point+5 , 0, colour);
    }

//copy_surface_to_image(wfall_buf,loc_x,loc_y);

//Scroll all lines up, starting from the bottom
for(int ll = WFALL_HEIGHT; ll >=0 ; ll--)
    {
    for(int pp = 0;pp<WFALL_WIDTH;pp++)
        {
        wfall_buf[((ll+1)*WFALL_WIDTH)+WFALL_WIDTH+pp] = wfall_buf[((ll)* WFALL_WIDTH)+pp];
        }
    }

copy_surface_to_image(wfall_buf,50,0,WFALL_WIDTH,WFALL_HEIGHT); // (buf,loc_x,lox_y,sz_x,sz_y)

}
#endif

#if 0
/*
void xxxdraw_fft()
{
int loc_x,loc_y;
int dummy;
int last;
int db_lev;

loc_x=10;
loc_y=10;
last = 255;
db_lev = 255;

fill_surface(&specanz,rgb565(0x01,0x07,0x01));
draw_grid();

for(int iii = 4; iii<1040;iii+=1)
    {
    db_lev=fft_video_buf[iii];
    plot_line(&specanz,iii,255,iii,db_lev+5,BLUE);

    plot_line(&specanz,iii,last,iii,db_lev,WHITE);
    last = db_lev;
    }

usleep(100*mS); //FIXME - rate should be controled elsewhere

ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
copy_surface_to_image(&specanz,loc_x,loc_y);
refresh_screen();
//ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync
}

*/





#endif

//=========
//-------------

void main()
{
unsigned int red,green,blue;
short rgba;
int screenbytes;
int quit_request;
int err;
int nv;
//int dummy;
__u32 dummy = 0;


nv=0;

fbfd = open("/dev/fb0", O_RDWR); // Open the framebuffer device file for reading and writing
if (fbfd == -1) 
    printf("Error: cannot open framebuffer device.\n");
 
if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) // Get variable screen information
	    printf("Error reading variable screen info.\n");
printf("Display info %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

screen_size_x = vinfo.xres;
screen_size_y = vinfo.yres;

int fb_data_size = 800 * 480 * 4;

scope_buf = malloc(SCOPE_WIDTH*SCOPE_HEIGHT*4);
wfall_buf = malloc(WFALL_WIDTH*WFALL_HEIGHT*4);
// map framebuffer to user memory 
frame_buf = mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
//close(fbfd); //It seems we can close this straight away (see Man)

clear_screen(0x00000000);

plot_large_string(frame_buf,360,300,"SCOPE",WHITE);

plot_thick_rectangle(scope_buf,0,0,700,SCOPE_HEIGHT-50,BLUE);


             

draw_grid();

plot_line(scope_buf,350,230,350,250,RED);


   while(1)
                {

//https://forums.raspberrypi.com/viewtopic.php?t=19073

//https://raspberrycompote.blogspot.com/2014/03/low-level-graphics-on-raspberry-pi-part_14.html
//https://raspberrycompote.blogspot.com/2014/03/low-level-graphics-on-raspberry-pi-part_16.html

for(int w=0;w<60;w++)
    err= ioctl(fbfd, FBIO_WAITFORVSYNC, &dummy); // Wait for frame sync

draw_grid();
copy_surface_to_image(scope_buf,50,0,SCOPE_WIDTH,SCOPE_HEIGHT); // (buf,loc_x,lox_y,sz_x,sz_y)


draw_fft();
//if(nv > 580) nv=1;


                }

printf(" Debug at %d\n",__LINE__);
sleep(1);
return ;

while(1) sleep(1); //stopper
}
