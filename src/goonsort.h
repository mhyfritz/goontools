#ifndef GOONSORT_H
#define GOONSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>

#include "array.h"
#include "ksort.h"
#include "goon.h"
#include "bgzf.h"

#include "goonsort.p"

#define MAX_MEM_MB_DEFAULT (size_t)756

typedef struct {
    int32_t pos;
    char *chrom, *json;
} Gn_sort_record_t;

typedef Gn_sort_record_t *Gn_sort_record_p;

typedef struct {
    char *seq_key,
         *start_key;
    size_t max_mem;
    int n_threads;
} Gn_sort_conf;

#endif
