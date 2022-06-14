#include <GLFW/glfw3.h>

#include "compute.h"
#include "io.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

GLFWwindow * init_glfw_context() {
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, 0);

    GLFWwindow * window = glfwCreateWindow(1, 1, "", NULL, NULL);
    if(window == NULL) {
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    return window;
}

void destroy_glfw_context(GLFWwindow * window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char * argv[]) {
    info_t * settings = read_settings(argc, argv);
    if(settings == NULL) return EXIT_FAILURE;

    FILE * out = init_output_file(settings);
    if(out == NULL) return EXIT_FAILURE;

    GLFWwindow * window = init_glfw_context();
    if(window == NULL) return EXIT_FAILURE;

    start_sim(settings, out);

    destroy_glfw_context(window);
    free_io(settings, out);

    return EXIT_SUCCESS;
}