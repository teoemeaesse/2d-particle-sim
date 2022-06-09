#ifndef __COMMON_H__
#define __COMMON_H__

#define ARG_FILE 1
#define ARG_PARTICLE_N 2
#define ARG_FRAMES 3

#define ERR_FOPEN(F) "[error]: Could not open file (%s).\n", F
#define ERR_FCLOSE(F) "[error]: Error closing file (%s).\n", F
#define ERR_FREAD(F) "[error]: Error reading from file (%s).\n", F
#define ERR_FTELL(F) "[error]: Could not find current cursor position of file (%s).\n", F
#define ERR_FSEEK(A, F) "[error]: Could not %s of file (%s).\n", A == SEEK_SET ? "seek start" : (A == SEEK_CUR ? "seek current cursor position" : "seek end"), F
#define ERR_FWRITE(F) "[error]: Error writing to file (%s).\n", F
#define ERR_INVALID_ARGC(F) "[error]: Invalid number of arguments (%d).\n", F
#define ERR_INVALID_PARTICLE_N(N) "[error]: Invalid particle count, please input an integer value (received %s).\n", N
#define ERR_INVALID_DURATION(T) "[error]: Invalid simulation duration, please input a positive integer value (received %s);\n", T
//#define ERR_INVALID_FRAMERATE(F) "[error]: Received invalid framerate value (%s).\n", F
#define ERR_SHADER_COMPILE(T) "[error]: Error compiling %s shader.\n", T == GL_VERTEX_SHADER ? "vertex" : (T == GL_COMPUTE_SHADER ? "compute" : "fragment")
#define ERR_EXPORT_FRAME(F) "[error]: Error exporting frame #%d.\n", F

#define INFO_USAGE "Usage: ./simulator [filename] [particle_count] [frames]\n"

#define ERROR(M, R) printf(M); return R
#define LOG(M) printf("%s", M);

#define LOG_INFO_FRAME(C, T, P) printf("[frame] %d / %d done (%f%%).\n", C, T, P)
#define LOG_SETTINGS(S) printf("[settings]: n = %d\n[settings]: t = %d\n", S->particle_count, S->frames)

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

// read in file as null-terminated string
char * read_file_as_string(const char * file);

#endif