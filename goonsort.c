#include "goonsort.h"

int goonsort(int argc, char *argv[])
{
    int i;

    printf("GOON sort\n");
    for (i = 0; i < argc; i += 1) {
        printf("  %s\n", argv[i]);
    }

    return 0;
}
