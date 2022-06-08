#ifndef __INPUT_H__
#define __INPUT_H__

typedef struct {
    int w_down, a_down, s_down, d_down, r_down, f_down, space_down, lshift_down;
} Keyboard;

void handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods);

void handle_scroll(GLFWwindow * window, double x, double y);

float normalize_zoom();

Keyboard get_keyboard();

float get_zoom();

void reset_zoom();

#endif