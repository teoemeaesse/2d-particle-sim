#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#define PARTICLE_SIZE 5 * sizeof(float)

typedef struct {
    char * filename;
    int particle_count, time, fps;
} settings_t;

typedef struct {
    settings_t * settings;
    int frame, framerate;
    float * particles;
} simulation_t;

simulation_t * load_simulation(int argc, char * argv[]);
void unload_simulation(simulation_t * sim);
void next_frame(simulation_t * sim);

#endif