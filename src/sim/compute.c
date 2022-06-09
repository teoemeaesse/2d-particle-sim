#include <GLFW/glfw3.h>

#include "compute.h"
#include "particle.h"
#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int export_frame(settings_t * settings, float * particles) {
    FILE * fp = fopen(settings->filename, "a");

    if(fp == NULL) {
        ERROR(ERR_FOPEN(settings->filename), -1);
    }
        
    if(fwrite(particles, PARTICLE_SIZE, settings->particle_count, fp) != settings->particle_count) {
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

unsigned int compile_shader(const char * src, unsigned int type) {
    unsigned int id = glCreateShader(type);
    
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char * message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        LOG(message);
        free(message);
        
        glDeleteShader(id);

        ERROR(ERR_SHADER_COMPILE(type), 0);
    }

    return id;
}

void gl_clear_errors() {
    unsigned int error;
    while(error = glGetError());
}

void gl_check_error() {
    unsigned int error;
    while((error = glGetError())) {
        printf("[opengl error]: %d\n", error);
    }
}

int start_sim(settings_t * settings) {
    char * src = read_file_as_string("shaders/newtonian_gravity.comp");
    unsigned int compute_shader = compile_shader(src, GL_COMPUTE_SHADER);
    free(src);
    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    glLinkProgram(compute_program);

    unsigned int ssbo_i, ssbo_f, loc_i, loc_f;
    glGenBuffers(1, &ssbo_i);
    glGenBuffers(1, &ssbo_f);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_i);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_f);
    loc_i = glGetProgramResourceIndex(compute_program, GL_SHADER_STORAGE_BLOCK, "i_data");
    loc_f = glGetProgramResourceIndex(compute_program, GL_SHADER_STORAGE_BLOCK, "f_data");
    glShaderStorageBlockBinding(compute_program, loc_i, 1);
    glShaderStorageBlockBinding(compute_program, loc_f, 2);
    
    float * particles = initialize_particles(settings->particle_count, square_initializer, uniform_mass_initializer),
          * updated_particles = PARTICLE_CALLOC(settings->particle_count);

    glNamedBufferData(ssbo_f, PARTICLE_SIZE * settings->particle_count, particles, GL_DYNAMIC_DRAW);
    
    for(int i = 0; i < settings->frames; i++) {
        glNamedBufferData(ssbo_i, PARTICLE_SIZE * settings->particle_count, i == 0 ? particles : updated_particles, GL_DYNAMIC_DRAW);

        glUseProgram(compute_program);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        
        glGetNamedBufferSubData(ssbo_f, 0, PARTICLE_SIZE * settings->particle_count, updated_particles);

        if(export_frame(settings, updated_particles) == -1) {
            ERROR(ERR_EXPORT_FRAME(i), -1);
        }
    }

    free(particles);

    return 0;
}