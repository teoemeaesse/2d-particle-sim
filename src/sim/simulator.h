#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

typedef struct {
    char * filename;
    int particle_count, time, fps;
} settings_t;

/* Creates output file and writes the simulation settings to it
*/
int init_output_file(settings_t * settings);

/* Reads command-line simulation settings
    return 0 for success, -1 for error
*/
int read_settings(settings_t * settings, int argc, char * argv[]);

// returns whether a null terminated string represents an integer
int isinteger(char * str);

#endif