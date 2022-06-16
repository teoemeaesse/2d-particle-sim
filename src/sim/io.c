#include "io.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

FILE * open_file(char * filename, const char * mode) {
    FILE * fp = fopen(filename, mode);

    if(fp == NULL) {
        ERROR(ERR_FOPEN(filename), NULL);
    }

    return fp;
}

int close_file(char * filename, FILE * fp) {
    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(filename), -1);
    }

    return 0;
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

settings_t * default_config() {
    settings_t * config = (settings_t *) malloc(sizeof(settings_t));

    config->filename = NULL;
    config->frames = DEFAULT_FRAMES;
    config->g = DEFAULT_G;
    config->invocations = DEFAULT_INVOCATIONS;
    config->n = DEFAULT_N;
    config->particle_attr_c = DEFAULT_PARTICLE_ATTR_COUNT;
    config->work_groups = DEFAULT_WORK_GROUPS;

    return config;
}

int assign_config_field(char * key, char * value, enum value_type valid_value, settings_t * config) {
    str_to_upper(key);

    if(str_in_arr(key, VALID_G_KEYS, VALID_G_KEYS_SIZE))
        return parse_float(&config->g, key, value, VALUE_FLT);

    if(str_in_arr(key, VALID_INVOCATIONS_KEYS, VALID_INVOCATIONS_KEYS_SIZE))
        return parse_integer(&config->invocations, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_N_KEYS, VALID_N_KEYS_SIZE))
        return parse_integer(&config->n, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_PARTICLE_SIZE_KEYS, VALID_PARTICLE_SIZE_KEYS_SIZE))
        return parse_integer(&config->particle_attr_c, key, value, VALUE_INT);

    if(str_in_arr(key, VALID_WORK_GROUPS_KEYS, VALID_WORK_GROUPS_KEYS_SIZE))
        return parse_integer(&config->work_groups, key, value, VALUE_INT);

    return -1;
}

settings_t * parse_config(char * filename) {
    FILE * fp = open_file(filename, "r");
    if(fp == NULL) return NULL;

    settings_t * config = default_config();

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
                    ERROR(ERR_INVALID_CONFIG(filename, buffer, l + 1), NULL);
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
                    if(assign_config_field(key, value, VALUE_INT, config) == -1) {
                        return NULL;
                    }
                    break;
                } 
                else if(!isdigit(c)) {
                    buffer[strlen(buffer) - 1] = '\0';
                    ERROR(ERR_INVALID_CONFIG(filename, buffer, l + 1), NULL);
                }
                value[k++] = c;
            }

            else if(state == PARSING_FLOAT) {
                if((c == '\n' || c == '\0' || toupper(c) == 'F') && value[k - 1] != '.') {
                    value[k] = '\0';
                    if(assign_config_field(key, value, VALUE_FLT, config) == -1)
                        return NULL;
                    break;
                }
                if(!isdigit(c)) {
                    buffer[strlen(buffer) - 1] = '\0';
                    ERROR(ERR_INVALID_CONFIG(filename, buffer, l + 1), NULL);
                }
                value[k++] = c;
            }

            else if(state == PARSING_STRING) {
                if(c == '\n' || c == '\0') {
                    value[k] = '\0';
                    if(assign_config_field(key, value, VALUE_STR, config) == -1)
                        return NULL;
                    break;
                }
                value[k++] = c;
            }
        }

        l++;
    }

    close_file(filename, fp);

    return config;
}

int free_io(settings_t * settings, FILE * fp) {
    if(close_file(settings->filename, fp) == -1) return -1;

    free(settings);

    return 0;
}

FILE * init_output_file(settings_t * settings) {
    FILE * fp = open_file(settings->filename, "w");
    if(fp == NULL) return NULL;

    if(fwrite(settings, sizeof(settings_t), 1, fp) != 1) {
        if(close_file(settings->filename, fp) == -1) return NULL;
        ERROR(ERR_FWRITE(settings->filename), NULL);
    }

    return fp;
}

settings_t * read_settings(int argc, char * argv[]) {
    if(argc != 3) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), NULL);
    }

    settings_t * settings = parse_config(argv[ARG_CONFIG_FILE]);
    if(settings == NULL) 
        return NULL;

    settings->filename = argv[ARG_OUTPUT_FILE];

    LOG_INFO_SETTINGS(settings);

    return settings;
}