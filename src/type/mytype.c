#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/mytype.h"

lupair* createLuPair(size_t first, size_t second) {
    lupair* p = (lupair*)malloc(sizeof(lupair));
    p->_first = first;
    p->_second = second;
    return p;
}