#include "simulation.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

simulation_t * create_simulation() {
    simulation_t * sim = (simulation_t *) malloc(sizeof(simulation_t));
    sim->settings = (settings_t *) malloc(sizeof(settings_t));
    sim->particles = NULL;

    return sim;
}

simulation_t * load_simulation(int argc, char * argv[]) {
    if(argc < 2) {
        printf("[load_simulation]: Invalid number of arguments (received %d)...", argc);
        return NULL;
    }


    FILE * fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("[load_simulation]: Could not open file _%s_; terminating...\n", argv[1]);
        return NULL;
    }

    simulation_t * sim = create_simulation();

    size_t r;
    if((r = fread(sim->settings, sizeof(settings_t), 1, fp)) != 1) {
        printf("[load_simulation]: Error reading simulation settings; terminating...\n");
        unload_simulation(sim);
        return NULL;
    }

    size_t frame_size = sim->settings->particle_count * PARTICLE_SIZE,
           frame_count = sim->settings->fps * sim->settings->time;
    sim->particles = (float *) malloc(frame_size * frame_count);

    if((r = fread(sim->particles, frame_size, frame_count, fp)) != frame_count) {
        printf("[load_simulation]: Error reading simulation (read %ld kb, expected %ld kb); terminating...\n", r * frame_size / 1024, frame_size * frame_count / 1024);
        unload_simulation(sim);
        return NULL;
    }

    fclose(fp);

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