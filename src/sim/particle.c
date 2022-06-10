#include "particle.h"
#include "common.h"

#include <math.h>
#include <stdlib.h>

#include <stdio.h>

float uniform_mass_initializer() {
    return (float) rand() / RAND_MAX * 5.0f;
}

float * initialize_particles(int n, void (* initialize_positions)(float *, int, float,float (* initialize_mass)()), float (* initialize_mass)()) {
    float * particles = PARTICLE_CALLOC(n);

    initialize_positions(particles, n, 250.0f, initialize_mass);

    return particles;
}

void square_initializer(float * particles, int n, float (* initialize_mass)()) {
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

void circular_initializer(float * particles, int n, float radius, float (* initialize_mass)()) {
    const float PHI = (1.0f + sqrt(5)) / 2.0f;

    for(int i = 0; i < n; i++) {
        float sq_dist = sqrt((float) i / (n - 1)),
              theta = 2 * M_PI * PHI * i,
              x = sq_dist * cos(theta) * radius,
              y = sq_dist * sin(theta) * radius;
        
        particles[i * 5] = x;
        particles[i * 5 + 1] = y;
        particles[i * 5 + 2] = -y / radius * PHI;
        particles[i * 5 + 3] = x / radius * PHI;
        particles[i * 5 + 4] = (* initialize_mass)();
    }
}