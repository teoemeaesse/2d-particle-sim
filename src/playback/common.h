#ifndef __COMMON_H__
#define __COMMON_H__

typedef struct {
    float x, y;
} Vec2;

#define ARG_FILE 1
#define ARG_FRAMERATE 2

#define ERR_FCLOSE(F) "[error]: Error closing file (%s).\n", F
#define ERR_FREAD(F) "[error]: Error reading from file (%s).\n", F
#define ERR_FTELL(F) "[error]: Could not find current cursor position of file (%s).\n", F
#define ERR_FSEEK(A, F) "[error]: Could not %s of file (%s).\n", A == SEEK_SET ? "seek start" : (A == SEEK_CUR ? "seek current cursor position" : "seek end"), F
#define ERR_FOPEN(F) "[error]: Could not open file (%s).\n", F
#define ERR_INVALID_ARGC(F) "[error]: Invalid number of arguments (%d).\n", F
#define ERR_RSIM(R, E) "[error]: Error reading simulation (read %ld kb, expected %ld kb).\n", R, E
#define ERR_INVALID_FRAMERATE(F) "[error]: Received invalid framerate value (%s).\n", F
#define ERR_SHADER_COMPILE(T) "[error]: Error compiling %s shader.\n", T == GL_VERTEX_SHADER ? "vertex" : (T == GL_COMPUTE_SHADER ? "compute" : "fragment")
#define ERR_SHADER_LINK "[error]: Error linking shader.\n"

#define INFO_USAGE "Usage: ./player [filename] [frames/s]\n"

#define ERROR(M, R) printf(M); return R
#define LOG(M) printf("%s", M);

#define LOG_LOAD(S) printf("[load_simulation]: n = %d\n[load_simulation]: t = %d\n[load_simulation]: simulation size = %ld kb\n", S->settings->particle_count, S->settings->frames, S->settings->particle_count * PARTICLE_SIZE * S->settings->frames / 1024)

// PIXELS / SECOND
#define PAN_SPEED 480.0f
#define PAN_SPEED_SHIFT 800.0f

// read in file as null-terminated string
char * read_file_as_string(const char * file);

// returns whether a null terminated string represents an integer
int is_integer(char * str);

#endif