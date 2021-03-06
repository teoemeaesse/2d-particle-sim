#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "io.h"

float uniform_mass_initializer();
float * initialize_particles(settings_t * settings, void (* initialize_positions)(float *, int, float, float (* initialize_mass0)()), float (* initialize_mass1)());
void square_initializer(float * particles, int n, float (* initialize_mass)());
void circular_initializer(float * particles, int n, float radius, float (* initialize_mass)());

#endif