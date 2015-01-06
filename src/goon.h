#ifndef GOON_H
#define GOON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    uint32_t pos;
    char *chrom;
} Chrom_pos;

#include "goon.p"

#endif
