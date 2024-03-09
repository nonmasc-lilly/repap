#include "../repap.h"

int main(int argc, char **argv) {
    window win;
    repap_init();
    create_window(win, 400, 400, "HELLO WINDOW!", 0x22EE11FF, 60);
    win_make_current(win);
    win_draw_rect(win, 100, 100, 200, 200, 0xFFFFFFFF);
    update_window(win);
    while(!window_closed(win)) {
        winloop_func(win);
    }
    destroy_window(win);
    repap_terminate();
}
