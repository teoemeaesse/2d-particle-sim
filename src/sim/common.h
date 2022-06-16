#ifndef __COMMON_H__
#define __COMMON_H__

enum value_type {VALUE_INT, VALUE_STR, VALUE_FLT};
enum parser_state {PARSING_NONE, PARSING_KEY, PARSING_NUMBER, PARSING_STRING, PARSING_WHITESPACE, PARSING_FLOAT, PARSING_INTEGER};

#define ARG_OUTPUT_FILE 1
#define ARG_CONFIG_FILE 2

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
#define ERR_INVALID_CONFIG(F, L, N) "[error]: Error reading config file (%s): Syntax error (line %d)\n\"%s\".\n", F, N, L
#define ERR_INVALID_VALUE(K, T) "[error]: Error parsing config file: %s should not be %s type.\n", K, T == VALUE_INT ? "integer" : (T == VALUE_STR ? "string" : "float")

#define INFO_USAGE "Usage: ./simulator [output file] [config file]\n"

#define ERROR(M, R) printf(M); return R
#define LOG(M) printf("%s", M);

#define LOG_INFO_FRAME(C, T, P) printf("\r[frame]: %d / %d done (%d%%).", C, T, P);
#define LOG_INFO_DONE(F) printf("\n[frame]: Finished exporting simulation to \"%s\"!\n", F);
#define LOG_INFO_SETTINGS(S) printf("[settings]: n = %d\n[settings]: t = %d\n", S->n, S->frames);
#define LOG_ERROR_GL(E) printf("[opengl]: Error code %d\n", E);

#define THREAD_COUNT 16
#define PARTICLE_SIZE 5 * sizeof(float)
#define PARTICLE_CALLOC(S) (float *) calloc(S, PARTICLE_SIZE)
#define GRAVITATIONAL_CONSTANT 0.00001f

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

extern const char * VALID_G_KEYS[16];
extern const char * VALID_INVOCATIONS_KEYS[16];
extern const char * VALID_N_KEYS[16];
extern const char * VALID_PARTICLE_SIZE_KEYS[32];
extern const char * VALID_WORK_GROUPS_KEYS[16];

// Returns whether a null terminated string represents an integer
int is_integer(char * str);

// Read in file as null-terminated string
char * read_file_as_string(const char * file);

// Capitalizes all the characters in str
void str_to_upper(char * str);

// Returns whether a string is contained in an array of strings of size size
int str_in_arr(char * str, const char ** arr, int size);

// Check for invalid type errors in key
int parse_float(float * ret, char * key, char * value, enum value_type valid_value);
int parse_integer(int * ret, char * key, char * value, enum value_type valid_value);
int parse_string(char * key, enum value_type valid_value);

#endif