#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>
#include "array.h"

ARRAY_DECLARE(char);
typedef Arraychar Line;

#define init_line(L) ARRAY_INIT(L, char, 4096)
#define store_line_char(L, C) ARRAY_PUSH(L, char, C)
#define free_line(L) ARRAY_FREE(L)

size_t readline(FILE *fp, Line *line);

#endif
