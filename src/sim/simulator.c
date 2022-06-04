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
        printf("[init_output_file]: Could not open file _%s_...\n", settings->filename);
        return -1;
    }

    fwrite(settings, sizeof(settings_t), 1, fp);

    fclose(fp);

    return 0;
}

// usage: ./simulator [filename] [particle_count] [t (seconds)] [frames/s]
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
        printf("[error]: invalid number of arguments (received %d);\n Usage: ./simulator [filename] [particle_count] [t (seconds)] [frames/s]\n", argc - 1);
        return -1;
    }
    
    if(isinteger(argv[2]) == -1) {
        printf("[error]: invalid particle count, please input an integer value (received %s);\n", argv[2]);
        return -1;
    }

    if(isinteger(argv[3]) == -1 || atoi(argv[3]) == 0) {
        printf("[error]: invalid simulation duration, please input a positive integer value (received %s);\n", argv[3]);
        return -1;
    }

    if(isinteger(argv[4]) == -1 || atoi(argv[4]) == 0) {
        printf("[error]: invalid framerate, please input a positive integer value (received %s);\n", argv[4]);
        return -1;
    }

    settings->filename = argv[1];
    settings->particle_count = atoi(argv[2]);
    settings->time = atoi(argv[3]);
    settings->fps = atoi(argv[4]);

    return 0;
}