#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>

typedef struct {
    char * filename;
    int n, frames, 
    work_groups, invocations,
     particle_attr_c;
    float g;
} settings_t;

/* Closes fp
*/
int close_file(char * filename, FILE * fp);

/* Look for @ token in source code to replace with config->invocations
    returns resulting source code
*/
char * inject_invocations(char * file, settings_t * config);

/* Parse simulator configuration from .config file
*/
settings_t * parse_config(char * filename);

/* Frees up memory allocated for settings and closes file (fp)
*/
int free_io(settings_t * settings, FILE * fp);

/* Creates output file and writes the simulation settings to it
*/
FILE * init_output_file(settings_t * settings);

/* Reads command-line simulation settings
*/
settings_t * read_settings(int argc, char * argv[]);

#endif