#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#include "simulator.h"

unsigned int compile_shader(const char * src, unsigned int type);
int start_sim(settings_t * settings);

#endif