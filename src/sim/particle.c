#include "particle.h"
#include "common.h"

#include <math.h>
#include <stdlib.h>

float uniform_mass_initializer() {
    return (float) ((double) rand() / (double) RAND_MAX * 5);
}

particle_t * initialize_particles(int n, void (* initialize_positions)(particle_t *, int, float (* initialize_mass)()), float (* initialize_mass)()) {
    particle_t * particles = PARTICLE_CALLOC(n);

    initialize_positions(particles, n, initialize_mass);

    return particles;
}

void square_initializer(particle_t * particles, int n, float (*initialize_mass)()) {
    int l = (int) sqrt((double) n);
    
    int i = 0;
    for(int x = 0; x < l; x++) {
        for(int y = 0; y < l; y++, i++) {
            particles[i].x = x * 4;
            particles[i].y = y * 4;
            particles[i].xv = 0;
            particles[i].yv = 0;
            particles[i].m = (* initialize_mass)();

            if(i == n)
                return;
        }
    }
}