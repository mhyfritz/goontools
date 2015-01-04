#include "readline.h"

size_t readline(FILE *fp, Line *line)
{
    char c;

    line->nextfree = 0;
    while ((c = getc(fp)) != EOF) {
        if (c == '\n') {
            store_line_char(line, '\0');
            break;
        }
        store_line_char(line, c);
    }

    return line->nextfree;
}
