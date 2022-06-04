#ifndef __COMMON_H__
#define __COMMON_H__

#define THREAD_COUNT 8
#define PARTICLE_SIZE 5 * sizeof(float)
#define PARTICLE_CALLOC(S) (float *) calloc(S, PARTICLE_SIZE)
#define GRAVITATIONAL_CONSTANT 0.01f

#define BUSY 1
#define IDLE 0
#define STOP 1
#define CONTINUE 0

// returns whether a null terminated string represents an integer
int isinteger(char * str);

#endif