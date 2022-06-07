#include "simulator.h"
#include "particle.h"
#include "concurrency.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

void print_settings(settings_t * settings) {
    printf("[settings]: n=%d\n", settings->particle_count);
    printf("[settings]: t=%d\n", settings->time);
    printf("[settings]: fps=%d\n", settings->fps);
}

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

int main(int argc, char * argv[]) {
    settings_t * settings = (settings_t *) calloc(1, sizeof(settings_t));
    if(read_settings(settings, argc, argv) == -1) {
        free(settings);
        return -1;
    }

    print_settings(settings);

    init_output_file(settings);

    float * particles = initialize_particles(settings->particle_count, square_initializer, uniform_mass_initializer);

    init_workers(particles, settings->particle_count, update_acc);

    int frames = settings->time * settings->fps;
    for(int i = 0; i < frames; i++) {
        export_frame(settings->filename);
        printf("[frame] %d / %d done.\n", i, frames);
    }

    join_workers();

    free(particles);
    free(settings);

    return 0;
}

int read_settings(settings_t * settings, int argc, char * argv[]) {
    if(argc != 5) {
        LOG(USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), -1);
    }
    
    if(!is_integer(argv[ARG_PARTICLE_N])) {
        ERROR(ERR_INVALID_PARTICLE_N(argv[ARG_PARTICLE_N]), -1);
    }

    if(!is_integer(argv[ARG_TIME]) || atoi(argv[ARG_TIME]) == 0) {
        ERROR(ERR_INVALID_DURATION(argv[ARG_TIME]), -1);
    }

    if(!is_integer(argv[ARG_FRAMERATE]) || atoi(argv[ARG_FRAMERATE]) == 0) {
        ERROR(ERR_INVALID_FRAMERATE(argv[ARG_FRAMERATE]), -1);
    }

    settings->filename = argv[ARG_FILE];
    settings->particle_count = atoi(argv[ARG_PARTICLE_N]);
    settings->time = atoi(argv[ARG_TIME]);
    settings->fps = atoi(argv[ARG_FRAMERATE]);

    return 0;
}