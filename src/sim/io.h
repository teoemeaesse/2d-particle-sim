#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>

typedef struct {
    char * filename;
    int particle_count, frames;
} settings_t;

/* Closes fp
*/
int close_file(settings_t * settings, FILE * fp);

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