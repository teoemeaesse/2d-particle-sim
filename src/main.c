#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "settings.h"
#include "player.h"
#include "simulator.h"

GLFWwindow * create_glfw_context() {
    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if(window == NULL) {
        glfwTerminate();
        return NULL;
    }

    return window;
}

void destroy_glfw_context(GLFWwindow * window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char * argv[]) {
    char opt, mode, c = 0;
    opterr = 0; 
    while((opt = getopt(argc, argv, "sp")) != -1) {
        c++;
        switch(opt) {
            case 's':
            case 'p':
                mode = opt;
                break;
            default:
                LOG(INFO_USAGE);
                ERROR(ERR_INVALID_FLAG, EXIT_FAILURE);
        }
    }

    if(c != 1) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_FLAGC(c), EXIT_FAILURE);
    }


    if(argc != 4) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), EXIT_FAILURE);
    }
    
    window_t window = {{WIDTH, HEIGHT}, create_glfw_context()};
    if(window.handle == NULL) return EXIT_FAILURE;

    switch(mode) {
        case 's':
            if(simulate(argc, argv, &window) == EXIT_FAILURE) {
                destroy_glfw_context(window.handle);
                return EXIT_FAILURE;
            }
            break;
        case 'p':
            if(play(argc, argv, &window) == EXIT_FAILURE) {
                destroy_glfw_context(window.handle);
                return EXIT_FAILURE;
            }
            break;
        default:
            destroy_glfw_context(window.handle);
            LOG(INFO_USAGE);
            ERROR(ERR_INVALID_FLAG, EXIT_FAILURE);
    }

    destroy_glfw_context(window.handle);

    return EXIT_SUCCESS;
}