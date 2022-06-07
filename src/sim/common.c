#include "common.h"

#include <ctype.h>

int is_integer(char * str) {
    while(*str != '\0')
        if(!isdigit(*(str++)))
            return 0;
    return 1;
}
