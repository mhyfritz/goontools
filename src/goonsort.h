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
#define TEMP_FILE_PREFIX_DEFAULT "gnsrt"

typedef struct {
    uint32_t pos;
    char *chrom;
} Chrom_pos;

#define POS_NULL UINT32_MAX

typedef struct {
    Chrom_pos *chrom_pos;
    char *json;
} Gn_sort_record_t;

typedef Gn_sort_record_t *Gn_sort_record_p;

typedef struct {
    Chrom_pos *chrom_pos;
    char *json;
    int i;
    size_t idx;
} Gn_merge_record_t;

typedef Gn_merge_record_t *Gn_merge_record_p;

#define HEAP_EMPTY POS_NULL

typedef struct {
    char *seq_key,
         *start_key,
         *prefix,
         *outfile;
    size_t max_mem;
    int n_threads;
} Gn_sort_conf;

#endif
