#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>
#include "array.h"

ARRAY_DECLARE(char);
typedef Arraychar Line;

#define init_line(L) ARRAY_INIT(L, char, 4096)
#define store_line_char(L, C) ARRAY_PUSH(L, char, C)
#define free_line(L) ARRAY_FREE(L)
#define line_realloc_check(L) ARRAY_REALLOC_CHECK(L, char)

size_t read_line(FILE *fp, Line *line);
void terminate_line(Line *line);

#endif
