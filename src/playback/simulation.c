#include "simulation.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

simulation_t * create_simulation(int framerate) {
    simulation_t * sim = (simulation_t *) malloc(sizeof(simulation_t));
    sim->settings = (settings_t *) malloc(sizeof(settings_t));
    sim->particles = NULL;
    sim->framerate = framerate;
    sim->frame = 0;

    return sim;
}

simulation_t * load_simulation(int argc, char * argv[]) {
    if(argc != 3) {
        ERROR(ERR_INVALID_ARGC(argc), NULL);
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
        unload_simulation(sim);
        ERROR(ERR_FREAD(argv[ARG_FILE]), NULL);
    }

    size_t frame_size = sim->settings->particle_count * PARTICLE_SIZE,
           frame_count = sim->settings->fps * sim->settings->time;
    sim->particles = (float *) malloc(frame_size * frame_count);

    size_t r = fread(sim->particles, frame_size, frame_count, fp);
    if(r != frame_count) {
        unload_simulation(sim);
        ERROR(ERR_RSIM(r * frame_size / 1024, frame_size * frame_count / 1024), NULL);
    }

    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(argv[ARG_FILE]), NULL);
    }

    printf("[load_simulation]: n=%d\n", sim->settings->particle_count);
    printf("[load_simulation]: t=%d\n", sim->settings->time);
    printf("[load_simulation]: fps=%d\n", sim->settings->fps);
    printf("[load_simulation]: simulation size (bytes)=%ld\n", frame_size * frame_count);
    
    return sim;
}

void unload_simulation(simulation_t * sim) {
    free(sim->settings);
    if(sim->particles != NULL)
        free(sim->particles);
    free(sim);
}

void next_frame(simulation_t * sim) {
    sim->frame++;

    if(sim->frame == sim->settings->fps * sim->settings->time) 
        sim->frame = 0;
}