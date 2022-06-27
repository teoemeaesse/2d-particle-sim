#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "simulator.h"
#include "particle.h"
#include "common.h"
#include "shader.h"

int export_frame(settings_t * settings, float * particles, FILE * out) {
    if(fwrite(particles, settings->particle_attr_c * sizeof(float), settings->n, out) != settings->n) {
        close_file(settings->filename, out);
        ERROR(ERR_FWRITE(settings->filename), -1);
    }

    return 0;
}

int start_sim(settings_t * settings, FILE * out) {
    char * src = read_file_as_string("shaders/newtonian_gravity.comp");
    char * v_src = inject_invocations(src, settings);
    unsigned int compute_shader = compile_shader(v_src, GL_COMPUTE_SHADER);
    free(src);
    free(v_src);
    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    link_program(compute_program);

    unsigned int ssbo_i, ssbo_f, loc_i, loc_f;
    glGenBuffers(1, &ssbo_i);
    glGenBuffers(1, &ssbo_f);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_i);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_f);
    loc_i = glGetProgramResourceIndex(compute_program, GL_SHADER_STORAGE_BLOCK, "i_data");
    loc_f = glGetProgramResourceIndex(compute_program, GL_SHADER_STORAGE_BLOCK, "f_data");
    glShaderStorageBlockBinding(compute_program, loc_i, 1);
    glShaderStorageBlockBinding(compute_program, loc_f, 2);

    size_t frame_size = settings->particle_attr_c * settings->n * sizeof(float);
    
    float * particles = initialize_particles(settings, circular_initializer, uniform_mass_initializer),
          * updated_particles = (float *) calloc(settings->n, settings->particle_attr_c * sizeof(float));

    glNamedBufferData(ssbo_f, frame_size, particles, GL_DYNAMIC_DRAW);

    log_wg_sizes();

    glUseProgram(compute_program);
    
    glNamedBufferData(ssbo_i, frame_size, particles, GL_DYNAMIC_DRAW);
    
    glUniform1i(glGetUniformLocation(compute_program, "n"), settings->n);
    glUniform1i(glGetUniformLocation(compute_program, "work_groups"), settings->work_groups);
    glUniform1i(glGetUniformLocation(compute_program, "particle_size"), settings->particle_attr_c);
    glUniform1f(glGetUniformLocation(compute_program, "g"), settings->g);

    int p = 0, lp = 0;
    for(int i = 0; i < settings->frames; i++) {
        if(i > 0) glNamedBufferSubData(ssbo_i, 0, frame_size, updated_particles);
    
        glDispatchCompute(settings->work_groups, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        
        glGetNamedBufferSubData(ssbo_f, 0, frame_size, updated_particles);

        if(export_frame(settings, updated_particles, out) == -1) {
            ERROR(ERR_EXPORT_FRAME(i), -1);
        }
        
        p = (int) ((float) i / settings->frames * 100.0f);
        if(p != lp) {
            LOG_INFO_FRAME(i, settings->frames, p);
            fflush(stdout);
        }
        lp = p;
    }

    LOG_INFO_DONE(settings->filename);
    gl_check_error();

    glDeleteShader(compute_shader);
    glDetachShader(compute_program, compute_shader);
    glDeleteProgram(compute_program);

    free(particles);
    free(updated_particles);
    
    return 0;
}

int simulate(int argc, char * argv[], window_t * w) {
    settings_t * settings = read_settings(argc, argv);
    if(settings == NULL) return EXIT_FAILURE;

    FILE * out = init_output_file(settings);
    if(out == NULL) return EXIT_FAILURE;

    glfwWindowHint(GLFW_VISIBLE, 0);
    glfwMakeContextCurrent(w->handle);
    start_sim(settings, out);

    free_io(settings, out);
    
    return EXIT_SUCCESS;
}