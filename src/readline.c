#include "readline.h"

void terminate_line(Line *line)
{
    line_realloc_check(line);
    line->elems[line->nextfree] = '\0';
}

size_t read_line(FILE *fp, Line *line)
{
    char c;

    line->nextfree = 0;
    while ((c = getc(fp)) != EOF) {
        if (c == '\n') {
            terminate_line(line);
            break;
        }
        store_line_char(line, c);
    }

    return line->nextfree;
}
