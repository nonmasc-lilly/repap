#include "inc/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef repap_h
#define repap_h

enum window_offsets {
    win_winp=0,
    win_vao=8,
    win_vbo=12,
    win_shader=16,
    win_texture=20,
    win_bg_color=24,
    win_pixels=27,
    win_width=35,
    win_height=39,
    win_deltatime=43,
    win_font=47,
    win_fps_max=55,
};
typedef enum {
    REFALSE,
    RETRUE
} REBOOL;
#define WINDOW_SIZE 59
typedef char window[WINDOW_SIZE];
/*
window {
    *winp; // window pointer, 8 bytes
    vao; // window vao, 4 bytes
    vbo; // window vbo, 4 bytes
    shader; // window shader 4 bytes
    texture; // window texture 4 bytes
    bg_color; // background colour 3 bytes
    *pixels; // pixel map, 8 bytes
    width; // width, 4 bytes
    height; // height, 4 bytes
    deltatime; // time between frames, 4 bytes
    **font; // window font, 8 bytes
    fps_max; // maximum fps for the window, 4 bytes
}
*/
enum sprite_offsets {
    spt_pixels=0,
    spt_width=8,
    spt_height=12
};
#define SPRITE_SIZE 16
typedef char spt[SPRITE_SIZE];
/*
sprite {
    *pixels; // sprites pixels 8 bytes
    width; //sprites width 4 bytes
    height; //sprites height 4 bytes
}
*/

void repap_init();
void create_window(window ret, unsigned int width,
    unsigned int height, const char *window_name, unsigned int bg_color, float fps_max);
void repap_terminate();
void destroy_window(window ret);
void win_make_current(window win);
void winloop_func(window win);
float win_get_deltatime(window win);
REBOOL window_closed(window win);
unsigned int color_from_bytes(char r, char g, char b, char a);
void putpixel(window win, unsigned int x, unsigned int y, unsigned int color);
unsigned int getpixel(window win, unsigned int x, unsigned int y);
void win_copy_pixel_buffer(window win, char *buf, unsigned int off, unsigned int len);
void win_draw_rect(window win, unsigned int x, unsigned int y, unsigned int width,
    unsigned int height, unsigned int color);
void win_draw_sprite(window win, unsigned int x, unsigned int y, spt sprite);
void win_draw_letter(window win, unsigned int x, unsigned int y, char oletter,
    unsigned int color);
void update_window(window win);
void font_from_file(const char *filepath, char ret[0xFF*8]);
void win_load_font(window win, char font[0xFF*8]);
#endif
