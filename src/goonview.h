#ifndef GOONVIEW_H
#define GOONVIEW_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>

#include "bgzf.h"
#include "tabix.h"

#include "goonview.p"

typedef struct {
    int8_t zerobased,
           onebased,
           rightopen,
           closed;
} Gn_view_conf;

#endif
