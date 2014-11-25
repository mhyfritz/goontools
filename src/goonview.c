#include "goonview.h"

#define PROG_ALIAS "goonview"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> <region1> [region2 [...]]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -f/--file     input goon.bgz file (mandatory)\n");
    fprintf(stderr, "\n");
}

int goonview(int argc, char *argv[])
{
    if (argc == 1) {
        USAGE;
        return -1;
    }

    return 0;
}
