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
    create_window(win, 400, 400, "HELLO WINDOW!", 0x22EE11FF);
    win_make_current(win);
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

here are the two main structures in repap:

`char window[47]` and `char spt[16]`

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
}
sprite {
    *pixels; //sprite pixels 8 bytes
    width; //sprites width 4 bytes
    height; //sprites height 4 bytes
}
```

repap also has one defined type:

```C
typedef enum {
    REFALSE=0,
    RETRUE=1
} REBOOL;
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
    unsigned int bg_color);
```

creates a new window of size `width` x `height`, with name
`window name` and with the background color of `bg_color`

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
void win_make_current(window win);
```

sets all of the applicable opengl settings to use `win` as
the current (vao, background color, current shader, as well
as calling glfwMakeContext current on `win`s `winp`

```C
void winloop_func(window win);
```

clears the screen, polls for input, draws `win` to the
screen and sets `win`s `deltatime` to be called every frame

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
    spt sprite);
```

draws `spt` onto `win` at `(x,y)`

```C
void update_window(window win);
```

takes the `pixels` of `win` and make them visible to opengl.




