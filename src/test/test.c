#include "../repap.h"

int main(int argc, char **argv) {
    window win;
    RETURN_TYPE rt;
    unsigned short input;
    repap_init();
    create_window(win, 400, 400, "HELLO WINDOW!", 0x22EE11FF, 60);
    win_make_current(&win);
    
    win_draw_rect(win, 100, 100, 200, 200, 0xFFFFFFFF);
    update_window(win);
    while(!window_closed(win)) {
        winloop_func(win);
        if(input_control(CT_CHECK_INPUT, NULL, &rt)) {
            input = input_control(CT_POP_INPUT, NULL, &rt);
            input &= 0xC0FF;
            if(input < 128 && input) {
                printf("%c", input);
                fflush(stdout);
                input = 0;
            }
        }
    }
    destroy_window(win);
    repap_terminate();
}
