#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>

typedef struct {
    int n, work_groups, invocations, particle_attr_c;
    float g;
} config_t;

typedef struct {
    char * filename;
    int particle_count, frames;
    config_t * config;
} info_t;

typedef struct {
    int key_count;
    int * key_sizes, * value_sizes;
    char ** keys;
    void ** values;
} serialization_buffer_t;

/* Closes fp
*/
int close_file(char * filename, FILE * fp);

/* Look for @ token in source code to replace with config->invocations
    returns resulting source code
*/
char * inject_invocations(char * file, config_t * config);

/* Parse simulator configuration from .config file
*/
config_t * parse_config(char * filename);

/* Frees up memory allocated for settings and closes file (fp)
*/
int free_io(info_t * settings, FILE * fp);

/* Creates output file and writes the simulation settings to it
*/
FILE * init_output_file(info_t * settings);

/* Reads command-line simulation settings
*/
info_t * read_settings(int argc, char * argv[]);

#endif