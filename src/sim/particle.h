#ifndef __PARTICLE_H__
#define __PARTICLE_H__

typedef struct {
    float x, y, xv, yv, m;
} particle_t;

float uniform_mass_initializer();
particle_t * initialize_particles(int n, void (* initialize_positions)(particle_t *, int, float (* initialize_mass0)()), float (* initialize_mass1)());
void square_initializer(particle_t * particles, int n, float (*initialize_mass)());
//void circular_initializer(float * particles, int n);

#endif