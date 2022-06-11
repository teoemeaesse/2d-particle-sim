#include "io.h"
#include "common.h"

#include "stdlib.h"

FILE * open_file(settings_t * settings, const char * mode) {
    FILE * fp = fopen(settings->filename, mode);

    if(fp == NULL) {
        ERROR(ERR_FOPEN(settings->filename), NULL);
    }

    return fp;
}

int close_file(settings_t * settings, FILE * fp) {
    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(settings->filename), -1);
    }

    return 0;
}

int free_io(settings_t * settings, FILE * fp) {
    if(close_file(settings, fp) == -1) return -1;

    free(settings);

    return 0;
}

FILE * init_output_file(settings_t * settings) {
    FILE * fp = open_file(settings, "w");

    if(fp == NULL) return NULL;

    if(fwrite(settings, sizeof(settings_t), 1, fp) != 1) {
        if(close_file(settings, fp) == -1) return NULL;

        ERROR(ERR_FWRITE(settings->filename), NULL);
    }

    return fp;
}

settings_t * read_settings(int argc, char * argv[]) {
    if(argc != 4) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), NULL);
    }
    
    if(!is_integer(argv[ARG_PARTICLE_N])) {
        ERROR(ERR_INVALID_PARTICLE_N(argv[ARG_PARTICLE_N]), NULL);
    }

    if(!is_integer(argv[ARG_FRAMES]) || atoi(argv[ARG_FRAMES]) == 0) {
        ERROR(ERR_INVALID_DURATION(argv[ARG_FRAMES]), NULL);
    }

    settings_t * settings = (settings_t *) calloc(1, sizeof(settings_t));

    settings->filename = argv[ARG_FILE];
    settings->particle_count = atoi(argv[ARG_PARTICLE_N]);
    settings->frames = atoi(argv[ARG_FRAMES]);

    LOG_INFO_SETTINGS(settings);

    return settings;
}