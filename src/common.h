#ifndef __COMMON_H__
#define __COMMON_H__

enum value_type {VALUE_INT, VALUE_STR, VALUE_FLT};
enum parser_state {PARSING_NONE, PARSING_KEY, PARSING_NUMBER, PARSING_STRING, PARSING_WHITESPACE, PARSING_FLOAT, PARSING_INTEGER};

#define ARG_OUTPUT_FILE 2
#define ARG_CONFIG_FILE 3
#define ARG_FILE 2
#define ARG_FRAMERATE 3

#define ERR_FOPEN(F) "[error]: Could not open file (%s).\n", F
#define ERR_FCLOSE(F) "[error]: Error closing file (%s).\n", F
#define ERR_FREAD(F) "[error]: Error reading from file (%s).\n", F
#define ERR_FTELL(F) "[error]: Could not find current cursor position of file (%s).\n", F
#define ERR_FSEEK(A, F) "[error]: Could not %s of file (%s).\n", A == SEEK_SET ? "seek start" : (A == SEEK_CUR ? "seek current cursor position" : "seek end"), F
#define ERR_FWRITE(F) "[error]: Error writing to file (%s).\n", F
#define ERR_INVALID_FRAMERATE(F) "[error]: Received invalid framerate value (%s).\n", F
#define ERR_INVALID_ARGC(F) "[error]: Invalid number of arguments (%d).\n", F
#define ERR_INVALID_PARTICLE_N(N) "[error]: Invalid particle count, please input an integer value (received %s).\n", N
#define ERR_INVALID_DURATION(T) "[error]: Invalid simulation duration, please input a positive integer value (received %s);\n", T
//#define ERR_INVALID_FRAMERATE(F) "[error]: Received invalid framerate value (%s).\n", F
#define ERR_SHADER_COMPILE(T) "[error]: Error compiling %s shader.\n", T == GL_VERTEX_SHADER ? "vertex" : (T == GL_COMPUTE_SHADER ? "compute" : "fragment")
#define ERR_SHADER_LINK "[error]: Error linking shader.\n"
#define ERR_EXPORT_FRAME(F) "[error]: Error exporting frame #%d.\n", F
#define ERR_INVALID_SETTINGS(F, L, N) "[error]: Error reading config file (%s): Syntax error (line %d)\n\"%s\".\n", F, N, L
#define ERR_INVALID_VALUE(K, T) "[error]: Error parsing config file: %s should not be %s type.\n", K, T == VALUE_INT ? "integer" : (T == VALUE_STR ? "string" : "float")
#define ERR_RSIM(R, E) "[error]: Error reading simulation (read %ld kb, expected %ld kb).\n", R, E
#define ERR_INVALID_FLAG "[error]: Invalid flag.\n"
#define ERR_INVALID_FLAGC(C) "[error]: Invalid flag count (%c).\n", C

#define INFO_USAGE "Usage: ./main -s [output file] [config file] # simulation mode\n       ./main -p [input file] [framerate] # playback mode\n"

#define ERROR(M, R) printf(M); return R
#define LOG(M) printf("%s", M);

#define LOG_LOAD(S) printf("[load_simulation]: n = %d\n[load_simulation]: t = %d\n[load_simulation]: simulation size = %ld kb\n", S->settings->n, S->settings->frames, S->settings->n * sim->settings->particle_attr_c * sizeof(float) * S->settings->frames / 1000)
#define LOG_INFO_FRAME(C, T, P) printf("\r[frame]: %d / %d done (%d%%).", C, T, P);
#define LOG_INFO_DONE(F) printf("\n[frame]: Finished exporting simulation to \"%s\"!\n", F);
#define LOG_INFO_SETTINGS(S) printf("[settings]: n = %d\n[settings]: t = %d\n", S->n, S->frames);
#define LOG_ERROR_GL(E) printf("[opengl]: Error code %d\n", E);
#define LOG_UNRECOGNIZED_CONFIG_KEY(T) printf("[log]: Unrecognized symbol (%s) in config file.\n", T);
#define LOG_COMPUTE_MAX(I, W) printf("[debug]: Invocations: ( %d ; %d ; %d )\n[debug]: Work groups: ( %d ; %d ; %d )\n", I[0], I[1], I[2], W[0], W[1], W[2]);


#define WIDTH 800
#define HEIGHT 600
#define TITLE "Simulation Player"

// PIXELS / SECOND
#define PAN_SPEED 480.0f
#define PAN_SPEED_SHIFT 800.0f

#define DEFAULT_PARTICLE_ATTR_COUNT 5
#define DEFAULT_G 0.01f
#define DEFAULT_WORK_GROUPS 64
#define DEFAULT_INVOCATIONS 64
#define DEFAULT_N 100
#define DEFAULT_FRAMES 300

#define VALID_G_KEYS_SIZE 2
#define VALID_INVOCATIONS_KEYS_SIZE 1
#define VALID_N_KEYS_SIZE 4
#define VALID_PARTICLE_SIZE_KEYS_SIZE 2
#define VALID_WORK_GROUPS_KEYS_SIZE 1
#define VALID_FRAMES_KEYS_SIZE 3


extern const char * VALID_G_KEYS[16];
extern const char * VALID_INVOCATIONS_KEYS[16];
extern const char * VALID_N_KEYS[16];
extern const char * VALID_PARTICLE_SIZE_KEYS[32];
extern const char * VALID_WORK_GROUPS_KEYS[16];
extern const char * VALID_FRAMES_KEYS[16];


// Returns whether a null terminated string represents an integer
int is_integer(char * str);

// Read in file as null-terminated string
char * read_file_as_string(const char * file);

// Capitalizes all the characters in str
void str_to_upper(char * str);

// Returns whether a string is contained in an array of strings of size size
int str_in_arr(char * str, const char ** arr, int size);

#endif