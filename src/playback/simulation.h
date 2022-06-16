#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#define PARTICLE_SIZE sizeof(particle_t)

typedef struct {
    char * filename;
    int n, frames, 
    work_groups, invocations,
     particle_attr_c;
    float g;
} settings_t;

typedef struct {
    float x, y, xv, yv, m;
} particle_t;

typedef struct {
    settings_t * settings;
    int frame, framerate, paused;
    particle_t * particles;
} simulation_t;

simulation_t * load_simulation(int argc, char * argv[]);
void unload_simulation(simulation_t * sim);
void next_frame(simulation_t * sim);

#endif