#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char * VALID_G_KEYS[16] = {"G", "GRAVITY"};
const char * VALID_INVOCATIONS_KEYS[16] = {"INVOCATIONS"};
const char * VALID_N_KEYS[16] = {"N", "PARTICLES", "COUNT", "PARTICLE_COUNT"};
const char * VALID_PARTICLE_SIZE_KEYS[32] = {"PARTICLE_SIZE", "PARTICLE_ATTRIBUTES"};
const char * VALID_WORK_GROUPS_KEYS[16] = {"WORK_GROUPS"};

int is_integer(char * str) {
    while(*str != '\0')
        if(!isdigit(*(str++)))
            return 0;
    return 1;
}

char * read_file_as_string(const char * file) {
    char * buffer = NULL;
    long length;
    FILE * fp;

    if((fp = fopen(file, "r")) == NULL) {
        ERROR(ERR_FOPEN(file), NULL);
    }

    if(fseek(fp, 0, SEEK_END) != 0) {
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(file), NULL);
        }
        ERROR(ERR_FSEEK(SEEK_END, file), NULL);
    }
    if((length = ftell(fp)) == -1L) {
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(file), NULL);
        }
        ERROR(ERR_FTELL(file), NULL);
    }
    if(fseek(fp, 0, SEEK_SET) != 0) {
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(file), NULL);
        }
        ERROR(ERR_FSEEK(SEEK_SET, file), NULL);
    }

    buffer = (char *) malloc(length * sizeof(char) + 1);
    if(buffer != NULL && fread(buffer, 1, length, fp) != length) {
        free(buffer);
        if(fclose(fp) != 0) {
            ERROR(ERR_FCLOSE(file), NULL);
        }
        ERROR(ERR_FREAD(file), NULL);
    }
    buffer[length] = '\0';

    if(fclose(fp) != 0) {
        ERROR(ERR_FCLOSE(file), NULL);
    }

    return buffer;
}

void str_to_upper(char * str) {
    for(int i = 0; i < strlen(str); i++)
        str[i] = toupper(str[i]);
}

int str_in_arr(char * str, const char ** arr, int size) {
    for(int i = 0; i < size; i++)
        if(strcmp(str, arr[i]) == 0)
            return 1;
    
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