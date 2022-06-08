#ifndef __COMMON_H__
#define __COMMON_H__

#define ARG_FILE 1
#define ARG_PARTICLE_N 2
#define ARG_TIME 3
#define ARG_FRAMERATE 4

#define ERR_FOPEN(F) "[error]: Could not open file (%s).\n", F
#define ERR_FCLOSE(F) "[error]: Error closing file (%s).\n", F
#define ERR_FWRITE(F) "[error]: Error writing to file (%s).\n", F
#define ERR_INVALID_ARGC(F) "[error]: Invalid number of arguments (%d).\n", F
#define ERR_INVALID_PARTICLE_N(N) "[error]: Invalid particle count, please input an integer value (received %s).\n", N
#define ERR_INVALID_DURATION(T) "[error]: Invalid simulation duration, please input a positive integer value (received %s);\n", T
#define ERR_INVALID_FRAMERATE(F) "[error]: Received invalid framerate value (%s).\n", F
#define INFO_USAGE "Usage: ./simulator [filename] [particle_count] [t (seconds)] [frames/s]\n"
#define LOG_INFO_FRAME(C, T) printf("[frame] %d / %d done.\n", C, T)

#define ERROR(M, R) printf(M); return R
#define LOG(M) printf("%s", M);

#define LOG_SETTINGS(S) printf("[settings]: n = %d\n[settings]: t = %d\n[settings]: fps = %d\n", S->particle_count, S->time, S->fps)

#define THREAD_COUNT 16
#define PARTICLE_SIZE 5 * sizeof(float)
#define PARTICLE_CALLOC(S) (float *) calloc(S, PARTICLE_SIZE)
#define GRAVITATIONAL_CONSTANT 0.00001f

#define BUSY 1
#define IDLE 0
#define STOP 1
#define CONTINUE 0

// returns whether a null terminated string represents an integer
int is_integer(char * str);

#endif