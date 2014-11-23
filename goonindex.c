#include "goonindex.h"

int goonindex(int argc, char *argv[])
{
    int i;

    printf("GOON index\n");
    for (i = 0; i < argc; i += 1) {
        printf("  %s\n", argv[i]);
    }

    return 0;
}
