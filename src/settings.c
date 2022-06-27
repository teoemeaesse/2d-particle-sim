#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "settings.h"
#include "io.h"

settings_t * default_settings() {
    settings_t * settings = (settings_t *) malloc(sizeof(settings_t));

    settings->filename = NULL;
    settings->frames = DEFAULT_FRAMES;
    settings->g = DEFAULT_G;
    settings->invocations = DEFAULT_INVOCATIONS;
    settings->n = DEFAULT_N;
    settings->particle_attr_c = DEFAULT_PARTICLE_ATTR_COUNT;
    settings->work_groups = DEFAULT_WORK_GROUPS;

    return settings;
}

int assign_settings_field(char * key, char * value, enum value_type valid_value, settings_t * settings) {
    str_to_upper(key);

    if(str_in_arr(key, VALID_G_KEYS, VALID_G_KEYS_SIZE))
        return parse_float(&settings->g, key, value, VALUE_FLT);

    if(str_in_arr(key, VALID_INVOCATIONS_KEYS, VALID_INVOCATIONS_KEYS_SIZE))
        return parse_integer(&settings->invocations, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_N_KEYS, VALID_N_KEYS_SIZE))
        return parse_integer(&settings->n, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_PARTICLE_SIZE_KEYS, VALID_PARTICLE_SIZE_KEYS_SIZE))
        return parse_integer(&settings->particle_attr_c, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_WORK_GROUPS_KEYS, VALID_WORK_GROUPS_KEYS_SIZE))
        return parse_integer(&settings->work_groups, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_FRAMES_KEYS, VALID_FRAMES_KEYS_SIZE))
        return parse_integer(&settings->frames, key, value, VALUE_INT);
    
    LOG_UNRECOGNIZED_CONFIG_KEY(key);

    return 0;
}

settings_t * parse_settings(char * filename) {
    FILE * fp = open_file(filename, "r");
    if(fp == NULL) return NULL;

    settings_t * settings = default_settings();

    const int BUFFER_SIZE = 64;
    char buffer[BUFFER_SIZE];
    int l = 0;
    while(fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        char c;
        char key[BUFFER_SIZE], value[BUFFER_SIZE];
        int state = PARSING_NONE;
        for(int i = 0, k = 0; i < strlen(buffer) + 1; i++) {
            c = buffer[i];

            if(state == PARSING_NONE) {
                if(c == '#' || c == '\n')
                    break;
                if(c != ' ') {
                    state = PARSING_KEY;
                    key[k++] = c;       
                }
            }
            
            else if(state == PARSING_KEY) {
                if(c == ':') {
                    state = PARSING_WHITESPACE;
                    key[k] = '\0';
                    continue;
                }
                else if(c == '#' || c == ' ') {
                    buffer[strlen(buffer) - 1] = '\0';
                    ERROR(ERR_INVALID_SETTINGS(filename, buffer, l + 1), NULL);
                }
                key[k++] = c;
            }
            
            else if(state == PARSING_WHITESPACE) {
                if(c == ' ')
                    continue;
                if(isdigit(c))
                    state = PARSING_NUMBER;
                else
                    state = PARSING_STRING;
                k = 0;
                value[k++] = c;
            }

            else if(state == PARSING_NUMBER) {
                if(c == '.') {
                    state = PARSING_FLOAT;
                }
                else if(c == '\n' || c == '\0' || c == ' ') {
                    value[k] = '\0';
                    if(assign_settings_field(key, value, VALUE_INT, settings) == -1) {
                        return NULL;
                    }
                    break;
                } 
                else if(!isdigit(c)) {
                    buffer[strlen(buffer) - 1] = '\0';
                    ERROR(ERR_INVALID_SETTINGS(filename, buffer, l + 1), NULL);
                }
                value[k++] = c;
            }

            else if(state == PARSING_FLOAT) {
                if((c == '\n' || c == '\0' || toupper(c) == 'F') && value[k - 1] != '.') {
                    value[k] = '\0';
                    if(assign_settings_field(key, value, VALUE_FLT, settings) == -1)
                        return NULL;
                    break;
                }
                if(!isdigit(c)) {
                    buffer[strlen(buffer) - 1] = '\0';
                    ERROR(ERR_INVALID_SETTINGS(filename, buffer, l + 1), NULL);
                }
                value[k++] = c;
            }

            else if(state == PARSING_STRING) {
                if(c == '\n' || c == '\0') {
                    value[k] = '\0';
                    if(assign_settings_field(key, value, VALUE_STR, settings) == -1)
                        return NULL;
                    break;
                }
                value[k++] = c;
            }
        }

        l++;
    }

    close_file(filename, fp);

    return settings;
}

char * inject_invocations(char * file, settings_t * config) {
    int digits = floor(log10(abs(config->invocations)) + 1);
    char * v_src = (char *) malloc((strlen(file) + digits) * sizeof(char)),
         * invocations_str = (char *) malloc(digits * sizeof(char));
    
    sprintf(invocations_str, "%d", config->invocations);

    int i = 0;
    while(i < strlen(file))
        if(file[i++] == '@')
            break;
    i--;
    file[i] = ' ';

    strncpy(v_src, file, i);
    v_src[i] = '\0';
    strcat(v_src, invocations_str);
    strcat(v_src, file + i);

    free(invocations_str);

    return v_src;
}

settings_t * read_settings(int argc, char * argv[]) {
    settings_t * settings = parse_settings(argv[ARG_CONFIG_FILE]);
    if(settings == NULL) 
        return NULL;

    settings->filename = argv[ARG_OUTPUT_FILE];

    LOG_INFO_SETTINGS(settings);

    return settings;
}

int free_io(settings_t * settings, FILE * fp) {
    if(close_file(settings->filename, fp) == -1) return -1;

    free(settings);

    return 0;
}

int parse_float(float * ret, char * key, char * value, enum value_type valid_value) {
    if(valid_value != VALUE_FLT) {
        ERROR(ERR_INVALID_VALUE(key, valid_value), -1);
    }

    *ret = atof(value);
    return 0;
}

int parse_integer(int * ret, char * key, char * value, enum value_type valid_value) {
    if(valid_value != VALUE_INT) {
        ERROR(ERR_INVALID_VALUE(key, valid_value), -1);
    }
    
    *ret = atoi(value);
    return 0;
}

int parse_string(char * key, enum value_type valid_value) {
    if(valid_value != VALUE_STR) {
        ERROR(ERR_INVALID_VALUE(key, valid_value), -1);
    }
    
    return 0;
}
