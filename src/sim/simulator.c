#include <GLFW/glfw3.h>

#include "simulator.h"
#include "particle.h"
#include "concurrency.h"
#include "compute.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

int init_output_file(settings_t * settings) {
    FILE * fp = fopen(settings->filename, "w");

    if(fp == NULL) {
        ERROR(ERR_FOPEN(settings->filename), -1);
    }

    if(fwrite(settings, sizeof(settings_t), 1, fp) != 1) {
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(settings->filename), -1);
        }
        ERROR(ERR_FWRITE(settings->filename), -1);
    }

    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(settings->filename), -1);
    }

    return 0;
}
void gl_check_error() {
    unsigned int error;
    while((error = glGetError())) {
        printf("[opengl error]: %d\n", error);
    }
}
int main(int argc, char * argv[]) {
    settings_t * settings = (settings_t *) calloc(1, sizeof(settings_t));
    if(read_settings(settings, argc, argv) == -1) {
        free(settings);
        return -1;
    }

    LOG_SETTINGS(settings);

    init_output_file(settings);

    if (!glfwInit())
        return 1;
        
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, 0);

    GLFWwindow * w = glfwCreateWindow(100, 100, "", NULL, NULL);
    if(w == NULL) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);

    char * src = read_file_as_string("shaders/newtonian_gravity.comp");
    unsigned int compute_shader = compile_shader(src, GL_COMPUTE_SHADER);
    free(src);
    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    glLinkProgram(compute_program);

    int data[5] = {1, 2, 3, 4, 5};
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * sizeof(int), data, GL_DYNAMIC_DRAW);
    
    unsigned int loc = glGetProgramResourceIndex(compute_program, GL_SHADER_STORAGE_BLOCK, "data");
    glShaderStorageBlockBinding(compute_program, loc, 1);

    glUseProgram(compute_program);
    glDispatchCompute(64, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    //glGetNamedBufferSubData(ssbo, 0, 5 * sizeof(int), new_data);

    particle_t * particles = initialize_particles(settings->particle_count, square_initializer, uniform_mass_initializer);

    init_workers(particles, settings->particle_count, update_acc);

    for(int i = 0; i < settings->frames; i++) {
        export_frame(settings->filename);
        LOG_INFO_FRAME(i, settings->frames, ((float) ((int) (i / (float) settings->frames * 10000))) / 100.0f);
    }

    join_workers();

    free(particles);
    free(settings);

    glfwTerminate();

    return 0;
}

int read_settings(settings_t * settings, int argc, char * argv[]) {
    if(argc != 4) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), -1);
    }
    
    if(!is_integer(argv[ARG_PARTICLE_N])) {
        ERROR(ERR_INVALID_PARTICLE_N(argv[ARG_PARTICLE_N]), -1);
    }

    if(!is_integer(argv[ARG_FRAMES]) || atoi(argv[ARG_FRAMES]) == 0) {
        ERROR(ERR_INVALID_DURATION(argv[ARG_FRAMES]), -1);
    }

    settings->filename = argv[ARG_FILE];
    settings->particle_count = atoi(argv[ARG_PARTICLE_N]);
    settings->frames = atoi(argv[ARG_FRAMES]);

    return 0;
}