#ifndef GOONINDEX_H
#define GOONINDEX_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdint.h>

#include "bgzf.h"

#include "goonindex.p"

typedef struct {
    char *seq_key,
         *start_key,
         *end_key;
    uint8_t rightopen;
} Gn_index_conf;

#endif
