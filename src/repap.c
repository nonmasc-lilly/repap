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
    if(!is_glad) is_glad =
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ? 1 : 0;
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
}

void destroy_window(window ret) {
    free(*((char**)(ret + win_pixels)));
    *((char**)(ret + win_pixels)) = NULL;
    glfwDestroyWindow(*((GLFWwindow**)(ret + win_winp)));
    glDeleteProgram(*((unsigned int*)(ret + win_shader)));
    glDeleteBuffers(1, (unsigned int*)(ret + win_vbo));
    glDeleteTextures(1, (unsigned int*)(ret + win_texture));
    glDeleteVertexArrays(1, (unsigned int*)(ret+win_vao));
}

void win_make_current(window win) {
    glfwMakeContextCurrent(*(GLFWwindow**)(win + win_winp));
    glUseProgram(*((unsigned int*)(win + win_shader)));
    glBindVertexArray(*((unsigned int*)(win + win_vao)));
    glClearColor(((float)(*(char*)(win + win_bg_color)))/255,
                 ((float)(*(char*)(win + win_bg_color + 1)))/255,
                 ((float)(*(char*)(win + win_bg_color + 2)))/255,
                 1);
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
    printf("hello\n");
    letter = (*(char**)(win + win_font))+oletter*8;
    for(i=0; i<8; i++) {
        printf("%x: ", letter[i]);
        for(j=0; j<8; j++) {
            printf("%d", (letter[7-i] >> (7-j)) & 1);
            putpixel(win, j+x, i+y, color * ((letter[7-i] >> (7-j)) & 1));
        }
        printf("\n");
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
