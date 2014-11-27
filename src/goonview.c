#include "goonview.h"

#define PROG_ALIAS "goonview"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s [arguments] <goon.bgz> <region1> [[region2]...]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "\n");
}

static int retrieve_regions(tabix_t *t,
                            char *regions[],
                            int num_regions)
{
    int i, len;
    ti_iter_t iter;
    const char *s;

    for (i = 0; i < num_regions; i += 1) {
        int tid, beg, end;
        if (ti_parse_region(t->idx, regions[i], &tid, &beg, &end) == 0) {
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
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int c;
    char *fn_goon, *fn_idx;
    tabix_t *t;
    Gn_view_conf conf;

    conf.zerobased = conf.rightopen = 0;

    if (argc < 3) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "h",
                            long_options,
                            NULL)) != -1) {
        switch (c) {
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

    if (retrieve_regions(t,
                         &argv[optind+1],
                         argc - optind - 1) != 0) {
        ti_close(t);
        return -1;
    }

    ti_close(t);

    return 0;
}
