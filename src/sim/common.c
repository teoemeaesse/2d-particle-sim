#include "common.h"

#include <ctype.h>

int isinteger(char * str) {
    while(*str != '\0')
        if(!isdigit(*(str++)))
            return -1;
    return 0;
}
