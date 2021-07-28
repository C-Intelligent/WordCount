#ifndef _MYTYPE_H_
#define _MYTYPE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

typedef struct long_unsigned_pair {
    size_t _first;
    size_t _second;
} lupair;

lupair* createLuPair(size_t first, size_t second);

#endif