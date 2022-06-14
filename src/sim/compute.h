#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#include "io.h"

/* Compiles the shader at src and returns a handle to it
*/
unsigned int compile_shader(const char * src, unsigned int type);

/* Starts the simulation and exports the result to file out
*/
int start_sim(info_t * settings, FILE * out);

#endif