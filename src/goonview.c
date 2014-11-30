#include "goonview.h"

#define PROG_ALIAS "goonview"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s [arguments] <goon.bgz> <region1> [[region2]...]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -0/--zerobased  zero-based positions\n");
    fprintf(stderr, "    -1/--onebased   one-based positions\n");
    fprintf(stderr, "    -r/--rightopen  right-open positions\n");
    fprintf(stderr, "    -c/--closed     closed positions\n");
    fprintf(stderr, "\n");
}

static int retrieve_regions(tabix_t *t,
                            Gn_view_conf *conf,
                            char *regions[],
                            int num_regions)
{
    int i, len;
    ti_iter_t iter;
    const char *s;

    for (i = 0; i < num_regions; i += 1) {
        int tid, beg, end;
        if (ti_parse_region(t->idx, regions[i], &tid, &beg, &end) == 0) {
            if (conf->onebased) {
                if (beg > 0) {
                    beg -= 1;
                }
                if (conf->rightopen) {
                    end -= 1;
                }
            } else if (conf->closed && end != 1<<29) {
                end += 1;
            }
            iter = ti_queryi(t, tid, beg, end);
            while ((s = ti_read(t, iter, &len)) != 0) {
                fputs(s, stdout); fputc('\n', stdout);
            }
            ti_iter_destroy(iter);
        }
    }        

    return -1;
}

int goonview(int argc, char *argv[])
{
    struct stat f_stat;
    struct option long_options[] =
    {
        {"zerobased", no_argument, NULL, '0'},
        {"onebased", no_argument, NULL, '1'},
        {"rightopen", no_argument, NULL, 'r'},
        {"closed", no_argument, NULL, 'c'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int c;
    char *fn_goon, *fn_idx;
    tabix_t *t;
    Gn_view_conf conf;
    const ti_conf_t *ti_conf;

    conf.zerobased = conf.onebased = conf.rightopen = conf.closed = 0;

    if (argc < 3) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "01rch",
                            long_options,
                            NULL)) != -1) {
        switch (c) {
            case '0': conf.zerobased = 1;
                      break;
            case '1': conf.onebased = 1;
                      break;
            case 'r': conf.rightopen = 1;
                      break;
            case 'c': conf.closed = 1;
                      break;
            case 'h': USAGE;
                      return -1;
            default: return -1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "error: missing arguments\n");
        USAGE;
        return -1;
    }

    if (conf.zerobased == 1 && conf.onebased == 1) {
        fprintf(stderr, "error: mutually exclusive options `zerobased` and "
                        "`onebased`\n");
        return -1;
    }

    if (conf.rightopen == 1 && conf.closed == 1) {
        fprintf(stderr, "error: mutually exclusive options `rightopen` and "
                        "`closed`\n");
        return -1;
    }

    fn_goon = argv[optind];

    if (stat(fn_goon, &f_stat) != 0) {
        fprintf(stderr, "error: input file does not exist\n");
        return -1;
    }

    if (bgzf_is_bgzf(fn_goon) != 1) {
        fprintf(stderr, "error: input file is not in bgzip format\n");
        return -1;
    }

    fn_idx = malloc(strlen(fn_goon) + strlen(IDX_EXT) + 1);
    strcat(strcpy(fn_idx, fn_goon), IDX_EXT);

    // TODO check for old index, see tabix main.c
    
    if (stat(fn_idx, &f_stat) != 0) {
        fprintf(stderr, "error: index file does not exist\n");
        return -1;
    }

    free(fn_idx);

    if ((t = ti_open(fn_goon, 0)) == 0) {
        fprintf(stderr, "error: fail to open the data file.\n");
        return -1;
    }

    if (ti_lazy_index_load(t) < 0) {
        fprintf(stderr,"error: failed to load the index file.\n");
        return -1;
    }

    ti_conf = ti_get_conf(t->idx);

    // default to how positions are stored in actual file/index
    if (conf.zerobased == 0 && conf.onebased == 0) {
        if (ti_conf->zerobased) {
            conf.zerobased = 1;
        } else {
            conf.onebased = 1;
        }
    }

    if (conf.rightopen == 0 && conf.closed == 0) {
        if (ti_conf->rightopen) {
            conf.rightopen = 1;
        } else {
            conf.closed = 1;
        }
    }

    if (retrieve_regions(t,
                         &conf,
                         &argv[optind+1],
                         argc - optind - 1) != 0) {
        ti_close(t);
        return -1;
    }

    ti_close(t);

    return 0;
}
