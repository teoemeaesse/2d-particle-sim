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

char * inject_invocations(char * file, config_t * config) {
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

config_t * default_config() {
    config_t * config = (config_t *) malloc(sizeof(config_t));

    config->g = DEFAULT_G;
    config->invocations = DEFAULT_INVOCATIONS;
    config->n = DEFAULT_N;
    config->particle_attr_c = DEFAULT_PARTICLE_ATTR_COUNT;
    config->work_groups = DEFAULT_WORK_GROUPS;

    return config;
}

int assign_config_field(char * key, char * value, int value_type, config_t * config) {
    str_to_upper(key);

    if(strcmp(key, "G") == 0 || strcmp(key, "GRAVITY") == 0) {
        if(value_type != VALUE_FLT) {
            ERROR(ERR_INVALID_VALUE(key, value_type), -1);
        }
        config->g = atof(value);
    }

    else if(strcmp(key, "INVOCATIONS") == 0) {
        if(value_type != VALUE_INT) {
            ERROR(ERR_INVALID_VALUE(key, value_type), -1);
        }
        config->invocations = atoi(value);
    }

    else if(strcmp(key, "N") == 0 || strcmp(key, "PARTICLES") == 0 || strcmp(key, "COUNT") == 0) {
        if(value_type != VALUE_INT) {
            ERROR(ERR_INVALID_VALUE(key, value_type), -1);
        }
        config->n = atoi(value);
    }

    else if(strcmp(key, "PARTICLE_SIZE") == 0 || strcmp(key, "PARTICLE_ATTRIBUTES") == 0) {
        if(value_type != VALUE_INT) {
            ERROR(ERR_INVALID_VALUE(key, value_type), -1);
        }
        config->particle_attr_c = atoi(value);
    }

    else if(strcmp(key, "WORK_GROUPS") == 0) {
        if(value_type != VALUE_INT) {
            ERROR(ERR_INVALID_VALUE(key, value_type), -1);
        }
        config->work_groups = atoi(value);
    }

    return 0;
}

config_t * parse_config(char * filename) {
    FILE * fp = open_file(filename, "r");
    if(fp == NULL) return NULL;

    config_t * config = default_config();

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

int free_io(info_t * settings, FILE * fp) {
    if(close_file(settings->filename, fp) == -1) return -1;

    free(settings);

    return 0;
}

FILE * init_output_file(info_t * settings) {
    FILE * fp = open_file(settings->filename, "w");
    if(fp == NULL) return NULL;

    serialization_buffer_t * serialization_buffer = serialize_settings(settings);
    // TODO
    if(fwrite(settings, sizeof(info_t), 1, fp) != 1) {
        if(close_file(settings->filename, fp) == -1) return NULL;
        ERROR(ERR_FWRITE(settings->filename), NULL);
    }

    return fp;
}

info_t * read_settings(int argc, char * argv[]) {
    if(argc != 4) {
        LOG(INFO_USAGE);
        ERROR(ERR_INVALID_ARGC(argc - 1), NULL);
    }
    
    if(!is_integer(argv[ARG_PARTICLE_N])) {
        ERROR(ERR_INVALID_PARTICLE_N(argv[ARG_PARTICLE_N]), NULL);
    }

    if(!is_integer(argv[ARG_FRAMES]) || atoi(argv[ARG_FRAMES]) == 0) {
        ERROR(ERR_INVALID_DURATION(argv[ARG_FRAMES]), NULL);
    }

    info_t * settings = (info_t *) calloc(1, sizeof(info_t));

    settings->filename = argv[ARG_FILE];
    settings->particle_count = atoi(argv[ARG_PARTICLE_N]);
    settings->frames = atoi(argv[ARG_FRAMES]);

    char config_file[12] = "test.config";
    settings->config = parse_config(config_file);

    LOG_INFO_SETTINGS(settings);

    return settings;
}