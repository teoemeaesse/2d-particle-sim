#ifndef __PARTICLE_H__
#define __PARTICLE_H__

int uniform_mass_initializer();
float * initialize_particles(int n, void (* initialize_positions)(float *, int, int (*initialize_mass0)()), int (*initialize_mass1)());
void square_initializer(float * particles, int n, int (*initialize_mass)());
//void circular_initializer(float * particles, int n);

float get_x(float * particles, int index);
float get_y(float * particles, int index);
float get_xv(float * particles, int index);
float get_yv(float * particles, int index);
float get_mass(float * particles, int index);

void set_x(float * particles, int index, float x);
void set_y(float * particles, int index, float y);
void set_xv(float * particles, int index, float xv);
void set_yv(float * particles, int index, float yv);
void set_mass(float * particles, int index, float mass);

#endif