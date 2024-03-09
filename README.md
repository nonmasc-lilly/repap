# REPAP
#### @@ Lilly Tau

## Description

repap is a pixel mapping graphics library based off of my previous
work, opap, the difference being the structure, featureset, and
most importantly, the lack of structs in the code. The reason for
this is to be as easily interoperable with assembly as possible
considering I would like to be able to program graphical
applications in assembly.

## Example program

```C
#include <repap.h>

int main(int argc, char **argv) {
    window win;
    repap_init();
    create_window(win, 400, 400, "HELLO WINDOW!", 0x22EE11FF, 60);
    win_make_current(&win);
    win_draw_rect(win, 100, 100, 300, 300, 0xFFFFFFFF);
    update_window(win);
    while(!window_closed(win)) {
        winloop_func(win);
    }
    destroy_window(win);
    repap_terminate();
}
```

## Install

to install repap simply run `sudo make install` while in the repap
main directory, you can then include repap with
`#include <repap.h>` and link it with `-lrepap`

it shall be noted that GLFW3 is required to build repap

it is also reccomended that all files interacting with repap use
`-funsigned-char` in order to remain consistent with repap

## Documentation

NOTE: repap runs with unsigned char as default, this documentation
will reflect that.

here are the two main structures in repap:

`char window[67]` and `char spt[16]`

which are made up of the following:

```C
window {
    *winp; // window pointer 8 bytes
    vao; // window vertex attribute buffer 4 bytes
    vbo; // window vertex buffer object 4 bytes
    shader; // window shader id 4 bytes
    texture; // window texture id 4 bytes
    bg_color; // window background color 3 bytes
    *pixels; // window pixel map 8 bytes
    width; // window width 4 bytes
    height; // window height 4 bytes
    deltatime; // time between window draw frames 4 bytes
    *font; // active window font, 8 bytes uninitialized by default
    fps_max; // maximum rendering fps for the window 4 bytes
    win_inp_stack; // a 10 unsigned short long stack where input will be stored
}
sprite {
    *pixels; //sprite pixels 8 bytes
    width; //sprites width 4 bytes
    height; //sprites height 4 bytes
}
```

repap also has a few defined type:

```C
typedef enum {
    REFALSE=0,
    RETRUE=1
} REBOOL; // for use in boolean operations
typedef enum {
    CT_ASSIGN_WINDOW_CURRENT,
    CT_PUSH_INPUT,
    CT_CHECK_INPUT
} CONTROL_TYPE; // a control type for use with control functions
typedef enum {
    RT_VOID,
    RT_BYTE,
    RT_PAPSCII,
} RETURN_TYPE; // a return type for use with control functions
```

the functions of repap are as follows:

```C
void repap_init()
```

simply initiates the repap library

```C
void repap_terminate()
```

simply terminates the repap library

```C
void create_window(
    window ret,
    unsigned int width,
    unsigned int height,
    const char *window_name,
    unsigned int bg_color,
    float fps_max);
```

creates a new window of size `width` x `height`, with name
`window name`, with the background color of `bg_color`,
and with the maximum fps of `fps_max`

`ret` is the window created by `create_window`

NOTE: colors in repap are defined in terms of unsigned integers
each byte being a single section of the RGBA color. The color
is ordered as RGBA meaning 0xFF0000FF is red, 0x00FF00FF is green
0x0000FFFF is green and 0x00000000 is completely transperent.

```C
void destroy_window(window ret);
```

frees all of the components of `ret` from memory, making ret
unusable in the process.

```C
void win_make_current(window *win);
```

sets all of the applicable opengl settings to use `*win` as
the current (vao, background color, current shader, as well
as calling glfwMakeContext current on `win`s `winp`

```C
void winloop_func(window win);
```

clears the screen, polls for input, draws `win` to the
screen and sets `win`s `deltatime` to be called every frame

draws frames at the speed defined by `win`s `fps_max` or slower
this does not stall the program, meaning all other logic will
not be halted waiting for the window draw to sync up
*stares at vsync furiously*

```C
float win_get_deltatime(window win);
```

returns `win`s current `deltatime`

```C
REBOOL window_closed(window win);
```

returns RETRUE if `win` is closed and REFALSE if it is open

```C
unsigned int color_from_bytes(char r, char g, char b, char a);
```

returns an unsigned int (representing a color) with the value
denoted as r << 24 | g << 16 | b << 8 | a

```C
void putpixel(window win, unsigned int x, unsigned int y,
    unsigned int color);
```

draws a pixel of color `color` into `pixels` of `win` at `(x,y)`
where (0,0) is the bottom left corner. Positive x moving right on
the screen and positive y moving upwards.

NOTE: drawn pixels will not be shown on calls of `winloop_func`
until `update_window` is called

```C
unsigned int getpixel(window win, char *x, unsigned int y);
```

returns the pixel color of `win`s `pixels` at `(x,y)` see putpixel
for details on the coordinate system

```C
void win_copy_pixel_buffer(window win, char *buf, unsigned int off,
    unsigned int len);
```

copies a buffer buf directly (without respect to the coordinate
system) into `win`s `pixels` starting at offset `off` and ending
at offset `off+len`

```C
void win_draw_rect(window win, unsigned int x, unsigned int y,
    unsigned int width, unsigned int height);
```

draws a rectangle at (x, y) extending into the top right direction
by (width, height)

```C
void win_draw_sprite(window win, unsigned int x, unsigned int y,
    spt sprite);
```

draws `spt` onto `win` at `(x,y)`

```C
void win_draw_letter(window win, unsigned int x, unsigned int y,
    char oletter, unsigned int color);
```

draws the letter `oleter` onto window at (x,y) from the bottom
left corner, with color `color`.

```C
void update_window(window win);
```

takes the `pixels` of `win` and make them visible to opengl.

```C
void font_from_file(const char *filepath, char ret[0xFF*8]);
```

loads a font from a `compact font design` file, fonts are
written in a similar way to the Commodore 64 fonts, being
8x8 in size and taking up only 8 bytes, each pixel being
represented by a simple bit. The font is loaded from
file at path `filepath` into bytearray `ret`

```C
void win_load_font(window win, char font[0xFF*8]);
```

loads a font `font` (loaded by font\_from\_file) into `win`
for use with `win_draw_letter`

```C
long input_control(CONTROL_TYPE ct, void *a, RETURN_TYPE *rt);
```

when called, enacts the desired control type (in `ct) taking
paramater `a` returning a long encoded as the type placed in
`*rt` by the function.

in case of `ct == CT_ASSIGN_WINDOW_CURRENT` the window value stored at `a`
will be stored as the current window. This is done by `win_make_current`

in case of `ct == CT_PUSH_INPUT` pushes the unsigned short value stored
at `a` to the input stack

in case of `ct == CT_POP_INPUT` pops the top of the input stack off and
returns it

in case of `ct == CT_CHECK_INPUT` returns a byte with the smallest bit
set if there is a new item on the stack, and the highest bit set if their
is still unreadinput on the stack.

PAPSCII is the encoding of returned input characters, the first 127 values
are the same as ascii but onwards from that there are

```C
typedef enum {
    PAP_INSERT = 128,
    PAP_RIGHT  = 129,
    PAP_LEFT   = 130,
    PAP_DOWN   = 131,
    PAP_UP     = 132,
    PAP_PGUP   = 133,
    PAP_PGDOWN = 134,
    PAP_HOME   = 135,
    PAP_END    = 136,
    PAP_SCLCK  = 137,
    PAP_PRSCR  = 138,
    PAP_PAUSE  = 139,
    PAP_F1     = 140,
    PAP_F2     = 141,
    PAP_F3     = 142,
    PAP_F4     = 143,
    PAP_F5     = 144,
    PAP_F6     = 145,
    PAP_F7     = 146,
    PAP_F8     = 147,
    PAP_F9     = 148,
    PAP_F10    = 149,
    PAP_F11    = 150,
    PAP_F12    = 151,
    PAP_F13    = 152,
    PAP_F14    = 153,
    PAP_F15    = 154,
    PAP_F16    = 155,
    PAP_F17    = 156,
    PAP_F18    = 157,
    PAP_F19    = 158,
    PAP_F20    = 159,
    PAP_F21    = 161,
    PAP_F22    = 162,
    PAP_F23    = 163,
    PAP_F24    = 164,
    PAP_F25    = 165,
    PAP_MENU   = 166,
} PAPSCII;
```

```C
void default_key_callback(GLFWwindow*,int,int,int,int);
```

the key callback passed to glfw on glad initialization



