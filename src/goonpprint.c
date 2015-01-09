#include "goonpprint.h"

#define PROG_ALIAS "goonpprint"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> [goonfile]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -i/--indent     print indentation\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

int goonpprint(int argc, char *argv[])
{
    struct option opts[] = {
        {"indent", required_argument, NULL, 'i'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c, indent = 2;
    FILE *f;

    while ((c = getopt_long(argc,
                            argv,
                            "i:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 'i': indent = strtol(optarg, NULL, 0);
                      break;
            default: return -1;
        }
    }

    if (indent <= 0) {
        fprintf(stderr, "error: indent has to be positive integer\n");
        return -1;
    }

    if (optind == argc) {
        f = stdin;
    } else {
        if (stat(argv[optind], &f_stat) != 0) {
            fprintf(stderr, "error: input file does not exist\n");
            return -1;
        }

        if ((f = fopen(argv[optind], "r")) == NULL) {
            fprintf(stderr, "error: cannot open input file %s\n", argv[optind]);
            return -1;
        }
    }

    if (optind != argc) {
        fclose(f);
    }

    return 0;
} 
