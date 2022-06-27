#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "common.h"

keyboard_t keyboard;
camera_t camera;

FILE * open_file(char * filename, const char * mode) {
    FILE * fp = fopen(filename, mode);

    if(fp == NULL) {
        ERROR(ERR_FOPEN(filename), NULL);
    }

    return fp;
}

int close_file(char * filename, FILE * fp) {
    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(filename), -1);
    }

    return 0;
}

FILE * init_output_file(settings_t * settings) {
    FILE * fp = open_file(settings->filename, "w");
    if(fp == NULL) return NULL;

    if(fwrite(settings, sizeof(settings_t), 1, fp) != 1) {
        if(close_file(settings->filename, fp) == -1) return NULL;
        ERROR(ERR_FWRITE(settings->filename), NULL);
    }

    return fp;
}

simulation_t * create_simulation(int framerate) {
    simulation_t * sim = (simulation_t *) malloc(sizeof(simulation_t));
    sim->settings = (settings_t *) malloc(sizeof(settings_t));
    sim->particles = NULL;
    sim->framerate = framerate;
    sim->frame = 0;
    sim->paused = 0;

    return sim;
}

simulation_t * load_simulation(int argc, char * argv[]) {
    if(argc != 4) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), NULL);
    }

    FILE * fp = fopen(argv[ARG_FILE], "r");
    if(fp == NULL) {
        LOG(INFO_USAGE);
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(argv[ARG_FILE]), NULL);
        }
        ERROR(ERR_FOPEN(argv[ARG_FILE]), NULL);
    }

    if(!is_integer(argv[ARG_FRAMERATE])) {
        LOG(INFO_USAGE);
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(argv[ARG_FILE]), NULL);
        }
        ERROR(ERR_INVALID_FRAMERATE(argv[ARG_FRAMERATE]), NULL);
    }

    simulation_t * sim = create_simulation(atoi(argv[ARG_FRAMERATE]));

    if(fread(sim->settings, sizeof(settings_t), 1, fp) != 1) {
        free_simulation(sim);
        ERROR(ERR_FREAD(argv[ARG_FILE]), NULL);
    }

    size_t frame_size = sim->settings->n * sim->settings->particle_attr_c * sizeof(float);
    sim->particles = (float *) malloc(frame_size * sim->settings->frames);

    size_t r = fread(sim->particles, frame_size, sim->settings->frames, fp);
    if(r != sim->settings->frames) {
        free_simulation(sim);
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(argv[ARG_FILE]), NULL);
        }
        ERROR(ERR_RSIM(r * frame_size / 1024, frame_size * sim->settings->frames / 1024), NULL);
    }

    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(argv[ARG_FILE]), NULL);
    }

    LOG_LOAD(sim);
    
    return sim;
}

void free_simulation(simulation_t * sim) {
    free(sim->settings);
    if(sim->particles != NULL)
        free(sim->particles);
    free(sim);
}

void next_frame(simulation_t * sim) {
    sim->frame++;

    if(sim->frame == sim->settings->frames) 
        sim->frame = 0;
}


/*  -----   KEYBOARD & MOUSE INPUT  -----   */


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
    camera.zoom += y / abs(y);
    if(camera.zoom == 0.0f)
        camera.zoom = 1.0f * y / abs(y);
}

float normalize_zoom(float zoom) {
    if(zoom < 0)
        return 1.0f / -zoom;
    return zoom;
}

void pan_camera(float delta[2]) {
    camera.offset[0] += delta[0];
    camera.offset[1] += delta[1];
}

keyboard_t * get_keyboard() {
    return &keyboard;
}

camera_t * get_camera() {
    return &camera;
}