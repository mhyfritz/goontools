#ifndef GOONEXTRACT_H
#define GOONEXTRACT_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "klib/kson.h"
#include "bgzf.h"
#include "array.h"

#include "goonextract.p"

#define DEFAULT_DELIMITER "\t"

typedef struct {
    size_t start,
           end;
} Path;

ARRAY_DECLARE(Path);

#endif
