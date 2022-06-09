#include "particle.h"
#include "common.h"

#include <math.h>
#include <stdlib.h>

float uniform_mass_initializer() {
    return (float) ((double) rand() / (double) RAND_MAX * 5);
}

float * initialize_particles(int n, void (* initialize_positions)(float *, int, float (* initialize_mass)()), float (* initialize_mass)()) {
    float * particles = PARTICLE_CALLOC(n);

    initialize_positions(particles, n, initialize_mass);

    return particles;
}

void square_initializer(float * particles, int n, float (*initialize_mass)()) {
    int l = (int) sqrt((double) n);
    
    int i = 0;
    for(int x = 0; x < l; x++) {
        for(int y = 0; y < l; y++) {
            particles[i * 5] = x * 4;
            particles[i * 5 + 1] = y * 4;
            particles[i * 5 + 2] = 0;
            particles[i * 5 + 3] = 0;
            particles[i * 5 + 4] = (* initialize_mass)();

            if(i++ == n)
                return;
        }
    }
}