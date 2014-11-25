#include "goonview.h"

int goonview(int argc, char *argv[])
{
    int i;

    printf("GOON view\n");
    for (i = 0; i < argc; i += 1) {
        printf("  %s\n", argv[i]);
    }

    return 0;
}
