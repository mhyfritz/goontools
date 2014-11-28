#include "goonidxstats.h"

#define PROG_ALIAS "goonidxstats"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <goon.bgz>\n", prog);
    fprintf(stderr, "\n");
}

int goonidxstats(int argc, char *argv[])
{
    struct stat f_stat;
    char *fn_goon, *fn_idx;
    tabix_t *t;
    const ti_conf_t *ti_conf;

    if (argc != 2) {
        USAGE;
        return -1;
    }

    fn_goon = argv[1];

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

    printf("SEQUENCE_KEY  %s\n", ti_conf->sk);
    printf("START_KEY     %s\n", ti_conf->bk);
    printf("END_KEY       %s\n", ti_conf->ek ? ti_conf->ek : "");
    printf("ZERO_BASED    %s\n", ti_conf->zerobased ? "true" : "false");
    printf("RIGHT_OPEN    %s\n", ti_conf->zerobased ? "true" : "false");

    ti_close(t);

    return 0;
}
