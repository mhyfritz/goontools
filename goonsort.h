#ifndef GOONSORT_H
#define GOONSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>
#include <assert.h>

#include "array.h"
#include "ksort.h"
#include "goon.h"

#include "goonsort.p"

typedef struct {
    int32_t pos;
    char *chrom, *json;
} Gn_sort_record_t;

typedef Gn_sort_record_t *Gn_sort_record_p;

#endif
