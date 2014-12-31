#ifndef GOONTOOLS_H
#define GOONTOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "goonindex.h"
#include "goonsort.h"
#include "goonview.h"
#include "goonidxstat.h"
#include "goonextract.h"

typedef int (*Subcommand)(int, char**);

#endif
