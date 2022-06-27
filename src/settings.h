#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "common.h"

typedef struct {
    char * filename;
    int n, frames, 
    work_groups, invocations,
     particle_attr_c;
    float g;
} settings_t;

typedef struct {
    float dimensions[2];
    GLFWwindow * handle;
} window_t;


/* Look for @ token in source code to replace with config->invocations
    returns resulting source code
*/
char * inject_invocations(char * file, settings_t * config);

/* Reads command-line simulation settings
*/
settings_t * read_settings(int argc, char * argv[]);

/* Frees up memory allocated for settings and closes file (fp)
*/
int free_io(settings_t * settings, FILE * fp);

/* Parse file for simulation settings
*/
int parse_float(float * ret, char * key, char * value, enum value_type valid_value);
int parse_integer(int * ret, char * key, char * value, enum value_type valid_value);
int parse_string(char * key, enum value_type valid_value);

#endif