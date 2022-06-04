#include "particle.h"
#include "common.h"

#include <math.h>
#include <stdlib.h>

int uniform_mass_initializer() {
    return (double) rand() / (double) RAND_MAX * 5;
}

float * initialize_particles(int n, void (*initialize_positions)(float*, int, int (*initialize_mass)()), int (*initialize_mass)()) {
    float * particles = PARTICLE_CALLOC(n);

    initialize_positions(particles, n, initialize_mass);

    return particles;
}

void square_initializer(float * particles, int n, int (*initialize_mass)()) {
    int l = (int) sqrt((double) n);
    
    int i = 0;
    for(int x = 0; x < l; x++) {
        for(int y = 0; y < l; y++, i++) {
            particles[i * 5] = x * 4;
            particles[i * 5 + 1] = y * 4;
            particles[i * 5 + 2] = 0;
            particles[i * 5 + 3] = 0;
            particles[i * 5 + 4] = (*initialize_mass)();

            if(i == n)
                return;
        }
    }
}

float get_x(float * particles, int index) {
    return particles[index * 5];
}

float get_y(float * particles, int index) {
    return particles[index * 5 + 1];
}

float get_xv(float * particles, int index) {
    return particles[index * 5 + 2];
}

float get_yv(float * particles, int index) {
    return particles[index * 5 + 3];
}

float get_mass(float * particles, int index) {
    return particles[index * 5 + 4];
}

void set_x(float * particles, int index, float x) {
    particles[index * 5] = x;
}

void set_y(float * particles, int index, float y) {
    particles[index * 5 + 1] = y;
}

void set_xv(float * particles, int index, float xv) {
    particles[index * 5 + 2] = xv;
}

void set_yv(float * particles, int index, float yv) {
    particles[index * 5 + 3] = yv;
}

void set_mass(float * particles, int index, float mass) {
    particles[index * 5 + 4] = mass;
}