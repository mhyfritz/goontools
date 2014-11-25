#include "goonindex.h"

#define PROG_ALIAS "goonindex"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> <goonfile.bgz>\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -s/--seqkey     sequence key (mandatory)\n");
    fprintf(stderr, "    -b/--startkey   start position key (mandatory)\n");
    fprintf(stderr, "    -e/--endkey     end position key\n");
    fprintf(stderr, "    -r/--rightopen  right-open positions\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

int goonindex(int argc, char *argv[])
{
    struct option opts[] = {
        {"seqkey", required_argument, NULL, 's'},
        {"startkey", required_argument, NULL, 'b'},
        {"endkey", required_argument, NULL, 'e'},
        {"rightopen", required_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c;
    Gn_index_conf conf;

    if (argc == 1) {
        USAGE;
        return -1;
    }

    conf.seq_key = conf.start_key = conf.end_key = NULL;
    conf.rightopen = 0;

    while ((c = getopt_long(argc,
                            argv,
                            "s:b:e:rh",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 's': conf.seq_key = optarg;
                      break;
            case 'b': conf.start_key = optarg;
                      break;
            case 'e': conf.end_key = optarg;
                      break;
            case 'r': conf.rightopen = 1;
                      break;
            default: return -1;
        }
    }

    if (conf.seq_key == NULL || conf.start_key == NULL) {
        fprintf(stderr, "error: missing mandatory argument\n");
        return -1;
    }

    if (optind == argc) {
        fprintf(stderr, "error: missing file name\n");
        return -1;
    }

    if (stat(argv[optind], &f_stat) != 0) {
        fprintf(stderr, "error: input file does not exist\n");
        return -1;
    }

    if (bgzf_is_bgzf(argv[optind]) != 1) {
        fprintf(stderr, "error: input file is not in bgzip format\n");
        return -1;
    }

    return 0;
}
