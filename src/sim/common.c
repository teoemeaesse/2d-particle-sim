#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
