#include "repap.h"

const float vdata[] = {
     1.0,  1.0, 0.0,   1.0, 1.0,
     1.0, -1.0, 0.0,   1.0, 0.0,
    -1.0,  1.0, 0.0,   0.0, 1.0,
     1.0, -1.0, 0.0,   1.0, 0.0,
    -1.0, -1.0, 0.0,   0.0, 0.0,
    -1.0,  1.0, 0.0,   0.0, 1.0,
};

const char *shader_source_vertex =
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec2 texture_coords;\n"
"out vec2 TexCoords;\n"
"void main() {\n"
"   gl_Position = vec4(position, 1);\n"
"   TexCoords = texture_coords;\n"
"}\n",
*shader_source_fragment =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D input_texture;\n"
"void main() {\n"
"   color = texture(input_texture, TexCoords);\n"
"}\n";



void repap_init() {
    glfwInit();
}
void repap_terminate() {
    glfwTerminate();
}

void create_window(window ret, unsigned int width,
        unsigned int height, const char *window_name, unsigned int bg_color, float fps_max) {
    static unsigned int is_glad = 0;
    unsigned int vshader = 0, fshader = 0;
    int success=0;
    char infoLog[512];
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    *((GLFWwindow**)(ret + win_winp)) =
        glfwCreateWindow(width, height, window_name, NULL, NULL);
    glfwMakeContextCurrent(*((GLFWwindow**)ret));
    if(!is_glad) {
        is_glad = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ? 1 : 0;
        if(is_glad) glfwSetKeyCallback(*(GLFWwindow**)(ret+win_winp),default_key_callback);
    }
    glfwSwapInterval(0);
    glViewport(0,0,width,height);
    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &shader_source_vertex, NULL);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vshader, 512, NULL, infoLog);
        printf("vshader compile error: %s\n", infoLog);
        exit(1);
    }
    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &shader_source_fragment, NULL);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fshader, 512, NULL, infoLog);
        printf("fshader compile error: %s\n", infoLog);
        exit(1);
    }
    glGenVertexArrays(1, (unsigned int*)(ret + win_vao));
    glBindVertexArray(*((unsigned int*)(ret + win_vao)));
    glGenBuffers(1, (unsigned int*)(ret + win_vbo));
    glBindBuffer(GL_ARRAY_BUFFER, *((unsigned int*)(ret + win_vbo)));
    glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
    *((unsigned int*)(ret + win_shader)) = glCreateProgram();
    glAttachShader(*((unsigned int*)(ret + win_shader)), vshader);
    glAttachShader(*((unsigned int*)(ret + win_shader)), fshader);
    glLinkProgram(*((unsigned int*)(ret + win_shader)));
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    *((char *)(ret + win_bg_color)) = bg_color >> 24 & 0xFF;
    *((char *)(ret + win_bg_color + 1)) = bg_color >> 16 & 0xFF;
    *((char *)(ret + win_bg_color + 2)) = bg_color >> 8 & 0xFF;
    *((char**)(ret + win_pixels)) = calloc(width*height*4,1);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, (unsigned int*)(ret + win_texture));
    glBindTexture(GL_TEXTURE_2D, *((unsigned int*)(ret + win_texture)));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        *((char**)(ret + win_pixels)));
    glGenerateMipmap(GL_TEXTURE_2D);
    *((unsigned int*)(ret + win_width)) = width;
    *((unsigned int*)(ret + win_height)) = height;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    *((char***)(ret + win_font)) = NULL;
    *(float*)(ret + win_fps_max) = fps_max;
    *(unsigned short**)(ret + win_inp_stack) = calloc((INPUT_STACK_LENGTH+1)*sizeof(unsigned short), 1);
}

void destroy_window(window ret) {
    free(*((char**)(ret + win_pixels)));
    *((char**)(ret + win_pixels)) = NULL;
    free(*(short**)(ret + win_inp_stack));
    *(short**)(ret + win_inp_stack) = NULL;
    glfwDestroyWindow(*((GLFWwindow**)(ret + win_winp)));
    glDeleteProgram(*((unsigned int*)(ret + win_shader)));
    glDeleteBuffers(1, (unsigned int*)(ret + win_vbo));
    glDeleteTextures(1, (unsigned int*)(ret + win_texture));
    glDeleteVertexArrays(1, (unsigned int*)(ret+win_vao));
}

void win_make_current(window *win) {
    RETURN_TYPE rt;
    glfwMakeContextCurrent(*(GLFWwindow**)(*win + win_winp));
    glUseProgram(*((unsigned int*)(*win + win_shader)));
    glBindVertexArray(*((unsigned int*)(*win + win_vao)));
    glClearColor(((float)(*(char*)(*win + win_bg_color)))/255,
                 ((float)(*(char*)(*win + win_bg_color + 1)))/255,
                 ((float)(*(char*)(*win + win_bg_color + 2)))/255,
                 1);
    input_control(CT_ASSIGN_WINDOW_CURRENT, (void*)(win), &rt);
}

void winloop_func(window win) {
    static float ltime = 0, lframetime = 0;
    if(ltime == 0) ltime = glfwGetTime();
    glfwPollEvents();
    if((glfwGetTime() - lframetime) >= 1 / *((float*)(win + win_fps_max))) {
        glfwSwapBuffers(*((GLFWwindow**)(win + win_winp)));
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        lframetime = glfwGetTime();
    }
    *((float *)(win + win_deltatime)) = glfwGetTime() - ltime;
    ltime = glfwGetTime();
    
}

float win_get_deltatime(window win) {
    return *(float*)(win + win_deltatime);
}

REBOOL window_closed(window win) {
    return glfwWindowShouldClose(*(GLFWwindow**)(win + win_winp));
}

unsigned int color_from_bytes(char r, char g, char b, char a) {
    unsigned int ur=r, ub=b, ug=g, ua=a;
    return (ur << 24) | (ug << 16) | (ub << 8) | (ua);
}
void putpixel(window win, unsigned int x, unsigned int y, unsigned int color) {
    *(unsigned int*)(*((char**)(win + win_pixels))+(
        x*4 + (y*4*(*(unsigned int*)(win + win_width))))) = color;
}
unsigned int getpixel(window win, unsigned int x, unsigned int y) {
    return
        *(unsigned int *)(*((char**)(win + win_pixels))+(
            x*4+(y*4*(*(unsigned int*)(win + win_width)))));
}
void win_copy_pixel_buffer(window win, char *buf, unsigned int off, unsigned int len) {
    memcpy(*(char**)(win + win_pixels) + off, buf, len);
}
void win_draw_rect(window win, unsigned int x, unsigned int y, unsigned int width,
        unsigned int height, unsigned int color) {
    unsigned int i,j;
    for(i=y; i<y+height; i++) for(j=x; j<x+width; j++) putpixel(win, j, i, color);
}
void win_draw_sprite(window win, unsigned int x, unsigned int y, spt sprite) {
    unsigned int i,j;
    for(i=y; i<y+*(unsigned int*)(sprite + spt_height); i++) {
        for(j=x; j<x+*(unsigned int*)(sprite + spt_width); j++) {
            putpixel(win, j, i,
                (*((unsigned int**)(sprite + spt_pixels)))[(j-x)+(i-y)*
                     (*(unsigned int *)(sprite + spt_width))]);
        }
    }
}
void win_draw_letter(window win, unsigned int x, unsigned int y, char oletter,
        unsigned int color) {
    unsigned int i, j;
    char *letter;
    letter = (*(char**)(win + win_font))+oletter*8;
    for(i=0; i<8; i++) {
        for(j=0; j<8; j++) {
            putpixel(win, j+x, i+y, color * ((letter[7-i] >> (7-j)) & 1));
        }
    }
    
    
}
void update_window(window win) {
    glBindTexture(GL_TEXTURE_2D, *((unsigned int*)(win + win_texture)));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, *((unsigned int*)(win + win_width)),
        *((unsigned int*)(win + win_height)), GL_RGBA, GL_UNSIGNED_BYTE,
        *((char**)(win + win_pixels)));
}

void font_from_file(const char *filepath, char ret[0xFF*8]) {
    FILE *fp;
    fp = fopen(filepath, "r");
    if(fp == NULL) {
        printf("font (%s) doesnt exist\n", filepath);
    }
    fread(ret, 1, 0xFF*8, fp);
    fclose(fp);
}
void win_load_font(window win, char font[0xFF*8]) {
    *(char**)(win + win_font) = font;
}

long input_control(CONTROL_TYPE ct, void *a, RETURN_TYPE *rt) {
    static window *win = NULL;
    static unsigned short *winpstack;
    static REBOOL newinput = REFALSE;
    static REBOOL noinput = REFALSE;
    unsigned int i;
    long ret;
    *rt = RT_VOID;
    switch(ct) {
    case CT_ASSIGN_WINDOW_CURRENT:
        win = (window*)a;
        winpstack = *(unsigned short **)((*win) + win_inp_stack);
        break;
    case CT_PUSH_INPUT:
        for(i=INPUT_STACK_LENGTH-1; i>0; i--)
            winpstack[i] = winpstack[i-1];
        winpstack[0] = *(unsigned short*)a;
        newinput = RETRUE;
        noinput = REFALSE;
        break;
    case CT_POP_INPUT:
        *rt = RT_PAPSCII;
        ret = (long)(winpstack[0]);
        for(i=0; i<INPUT_STACK_LENGTH-1; i++) {
            winpstack[i] = winpstack[i+1];
            if(winpstack[i+1]) noinput = RETRUE;
        }
        winpstack[INPUT_STACK_LENGTH-1] = 0;
        newinput = REFALSE;
        return ret;
    case CT_CHECK_INPUT:
        *rt = RT_BYTE;
        ret = (noinput << 1) | newinput;
        return ret;
    default:
        printf("undefined input control\n");
        break;
    }
}

void default_key_callback(GLFWwindow *win, int key, int scancode, int action,
        int mods) {
    unsigned short x;
    RETURN_TYPE rt;
    if(action == GLFW_REPEAT) return;
    switch(key) {
    case GLFW_KEY_SPACE:         x = ' ';  break;
    case GLFW_KEY_APOSTROPHE:    x = '\''; break;
    case GLFW_KEY_COMMA:         x = ',';  break;
    case GLFW_KEY_MINUS:         x = '-';  break;
    case GLFW_KEY_PERIOD:        x = '.';  break;
    case GLFW_KEY_SLASH:         x = '/';  break;
    case GLFW_KEY_0:             x = '0';  break;
    case GLFW_KEY_1:             x = '1';  break;
    case GLFW_KEY_2:             x = '2';  break;
    case GLFW_KEY_3:             x = '3';  break;
    case GLFW_KEY_4:             x = '4';  break;
    case GLFW_KEY_5:             x = '5';  break;
    case GLFW_KEY_6:             x = '6';  break;
    case GLFW_KEY_7:             x = '7';  break;
    case GLFW_KEY_8:             x = '8';  break;
    case GLFW_KEY_9:             x = '9';  break;
    case GLFW_KEY_SEMICOLON:     x = ';';  break;
    case GLFW_KEY_EQUAL:         x = '=';  break;
    case GLFW_KEY_A:             x = 'A';  break;
    case GLFW_KEY_B:             x = 'B';  break;
    case GLFW_KEY_C:             x = 'C';  break;
    case GLFW_KEY_D:             x = 'D';  break;
    case GLFW_KEY_E:             x = 'E';  break;
    case GLFW_KEY_F:             x = 'F';  break;
    case GLFW_KEY_G:             x = 'G';  break;
    case GLFW_KEY_H:             x = 'H';  break;
    case GLFW_KEY_I:             x = 'I';  break;
    case GLFW_KEY_J:             x = 'J';  break;
    case GLFW_KEY_K:             x = 'K';  break;
    case GLFW_KEY_L:             x = 'L';  break;
    case GLFW_KEY_M:             x = 'M';  break;
    case GLFW_KEY_N:             x = 'N';  break;
    case GLFW_KEY_O:             x = 'O';  break;
    case GLFW_KEY_P:             x = 'P';  break;
    case GLFW_KEY_Q:             x = 'Q';  break;
    case GLFW_KEY_R:             x = 'R';  break;
    case GLFW_KEY_S:             x = 'S';  break;
    case GLFW_KEY_T:             x = 'T';  break;
    case GLFW_KEY_U:             x = 'U';  break;
    case GLFW_KEY_V:             x = 'V';  break;
    case GLFW_KEY_W:             x = 'W';  break;
    case GLFW_KEY_X:             x = 'X';  break;
    case GLFW_KEY_Y:             x = 'Y';  break;
    case GLFW_KEY_Z:             x = 'Z';  break;
    case GLFW_KEY_LEFT_BRACKET:  x = '[';  break;
    case GLFW_KEY_BACKSLASH:     x = '\\'; break;
    case GLFW_KEY_RIGHT_BRACKET: x = ']';  break;
    case GLFW_KEY_GRAVE_ACCENT:  x = '`';  break;
    case GLFW_KEY_ESCAPE:        x = 27;   break;
    case GLFW_KEY_ENTER:         x = '\n'; break;
    case GLFW_KEY_TAB:           x = '\t'; break;
    case GLFW_KEY_BACKSPACE:     x = '\b'; break;
    case GLFW_KEY_INSERT:        x = PAP_INSERT ;break;
    case GLFW_KEY_DELETE:        x = 127        ;break;
    case GLFW_KEY_RIGHT:         x = PAP_RIGHT  ;break;
    case GLFW_KEY_LEFT:          x = PAP_LEFT   ;break;
    case GLFW_KEY_DOWN:          x = PAP_DOWN   ;break;
    case GLFW_KEY_UP:            x = PAP_UP     ;break;
    case GLFW_KEY_PAGE_UP:       x = PAP_PGUP   ;break;
    case GLFW_KEY_PAGE_DOWN:     x = PAP_PGDOWN ;break;
    case GLFW_KEY_HOME:          x = PAP_HOME   ;break;
    case GLFW_KEY_END:           x = PAP_END    ;break;
    case GLFW_KEY_SCROLL_LOCK:   x = PAP_SCLCK  ;break;
    case GLFW_KEY_PRINT_SCREEN:  x = PAP_PRSCR  ;break;
    case GLFW_KEY_PAUSE:         x = PAP_PAUSE  ;break;
    case GLFW_KEY_F1:            x = PAP_F1     ;break;
    case GLFW_KEY_F2:            x = PAP_F2     ;break;
    case GLFW_KEY_F3:            x = PAP_F3     ;break;
    case GLFW_KEY_F4:            x = PAP_F4     ;break;
    case GLFW_KEY_F5:            x = PAP_F5     ;break;
    case GLFW_KEY_F6:            x = PAP_F6     ;break;
    case GLFW_KEY_F7:            x = PAP_F7     ;break;
    case GLFW_KEY_F8:            x = PAP_F8     ;break;
    case GLFW_KEY_F9:            x = PAP_F9     ;break;
    case GLFW_KEY_F10:           x = PAP_F10    ;break;
    case GLFW_KEY_F11:           x = PAP_F11    ;break;
    case GLFW_KEY_F12:           x = PAP_F12    ;break;
    case GLFW_KEY_F13:           x = PAP_F13    ;break;
    case GLFW_KEY_F14:           x = PAP_F14    ;break;
    case GLFW_KEY_F15:           x = PAP_F15    ;break;
    case GLFW_KEY_F16:           x = PAP_F16    ;break;
    case GLFW_KEY_F17:           x = PAP_F17    ;break;
    case GLFW_KEY_F18:           x = PAP_F18    ;break;
    case GLFW_KEY_F19:           x = PAP_F19    ;break;
    case GLFW_KEY_F20:           x = PAP_F20    ;break;
    case GLFW_KEY_F21:           x = PAP_F21    ;break;
    case GLFW_KEY_F22:           x = PAP_F22    ;break;
    case GLFW_KEY_F23:           x = PAP_F23    ;break;
    case GLFW_KEY_F24:           x = PAP_F24    ;break;
    case GLFW_KEY_F25:           x = PAP_F25    ;break;
    case GLFW_KEY_KP_0:          x = '0';  break;
    case GLFW_KEY_KP_1:          x = '1';  break;
    case GLFW_KEY_KP_2:          x = '2';  break;
    case GLFW_KEY_KP_3:          x = '3';  break;
    case GLFW_KEY_KP_4:          x = '4';  break;
    case GLFW_KEY_KP_5:          x = '5';  break;
    case GLFW_KEY_KP_6:          x = '6';  break;
    case GLFW_KEY_KP_7:          x = '7';  break;
    case GLFW_KEY_KP_8:          x = '8';  break;
    case GLFW_KEY_KP_9:          x = '9';  break;
    case GLFW_KEY_KP_DECIMAL:    x = '.';  break;
    case GLFW_KEY_KP_DIVIDE:     x = '/';  break;
    case GLFW_KEY_KP_MULTIPLY:   x = '*';  break;
    case GLFW_KEY_KP_SUBTRACT:   x = '-';  break;
    case GLFW_KEY_KP_ADD:        x = '+';  break;
    case GLFW_KEY_KP_ENTER:      x = '\n'; break;
    case GLFW_KEY_KP_EQUAL:      x = '=';  break;
    case GLFW_KEY_MENU:          x = PAP_MENU   ;break;
    default: return;
    }
    x |= (mods & 0x3F) << 8;
    x |= action == GLFW_PRESS ? 0x80 : 0x0;
    input_control(CT_PUSH_INPUT, (void*)&x, &rt);
}
