#include <GLFW/glfw3.h>

#include "input.h"

#include <stdio.h>
#include <stdlib.h>

Keyboard keyboard;
float zoom = 1.0f;

void handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS || action == GLFW_RELEASE) {
        if(key == GLFW_KEY_W)
            keyboard.w_down = action;
        if(key == GLFW_KEY_A)
            keyboard.a_down = action;
        if(key == GLFW_KEY_S)
            keyboard.s_down = action;
        if(key == GLFW_KEY_D)
            keyboard.d_down = action;
        if(key == GLFW_KEY_R)
            keyboard.r_down = action;
        if(key == GLFW_KEY_F)
            keyboard.f_down = action;
        if(key == GLFW_KEY_SPACE)
            keyboard.space_down = action;
        if(key == GLFW_KEY_LEFT_SHIFT)
            keyboard.lshift_down = action;
    }
}

void handle_scroll(GLFWwindow* window, double x, double y) {
    zoom += y / abs(y);
    if(zoom == 0.0f)
        zoom = 1.0f * y / abs(y);
}

float normalize_zoom() {
    if(zoom < 0)
        return 1.0f / -zoom;
    return zoom;
}

Keyboard get_keyboard() {
    return keyboard;
}

float get_zoom() {
    return zoom;
}

void reset_zoom() {
    zoom = 1.0f;
}