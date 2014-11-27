#include "goonindex.h"

#define PROG_ALIAS "goonindex"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> <goon.bgz>\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -s/--seqkey     sequence key (mandatory)\n");
    fprintf(stderr, "    -b/--startkey   start position key (mandatory)\n");
    fprintf(stderr, "    -e/--endkey     end position key\n");
    fprintf(stderr, "    -0/--zerobased  zero-based positions\n");
    fprintf(stderr, "    -r/--rightopen  right-open positions\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

static int build_gn_index(char *fn, ti_conf_t *conf)
{
    char *fnidx;

    fnidx = malloc(strlen(fn) + strlen(IDX_EXT) + 1);
    strcat(strcpy(fnidx, fn), IDX_EXT);

    return ti_index_build(fn, conf);
}

static int store_key(char *src, char *dest)
{
    if (strlen(src) > MAX_KEY_LEN) {
        fprintf(stderr, "error: key length limited to %d\n", MAX_KEY_LEN);
        return -1;
    }
    if (strcpy(dest, src) == NULL) {
        return -1;
    }
    return 0;
}

int goonindex(int argc, char *argv[])
{
    struct option opts[] = {
        {"seqkey", required_argument, NULL, 's'},
        {"startkey", required_argument, NULL, 'b'},
        {"endkey", required_argument, NULL, 'e'},
        {"zerobased", no_argument, NULL, '0'},
        {"rightopen", no_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c;
    ti_conf_t conf;

    if (argc == 1) {
        USAGE;
        return -1;
    }

    INIT_CONF(&conf);

    while ((c = getopt_long(argc,
                            argv,
                            "s:b:e:0rh",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 's': if (store_key(optarg, conf.sk) != 0) {
                          fprintf(stderr, "could not store key\n");
                          return -1;
                      }
                      break;
            case 'b': if (store_key(optarg, conf.bk) != 0) {
                          fprintf(stderr, "could not store key\n");
                          return -1;
                      }
                      break;
            case 'e': if (store_key(optarg, conf.ek) != 0) {
                          fprintf(stderr, "could not store key\n");
                          return -1;
                      }
                      break;
            case '0': conf.zerobased = 1;
                      break;
            case 'r': conf.rightopen = 1;
                      break;
            default: return -1;
        }
    }

    if (conf.sk == NULL || conf.sk == NULL) {
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

    return build_gn_index(argv[optind], &conf);

    return 0;
}
