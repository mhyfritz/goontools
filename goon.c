#include "goon.h"

void skip_ws(char **str)
{
    while (**str != '\0' && isspace(**str)) {
        *str += 1;
    }
}
