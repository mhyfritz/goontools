#include "goonidxstat.h"

#define PROG_ALIAS "goonidxstat"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <goon.bgz>\n", prog);
    fprintf(stderr, "\n");
}

int goonidxstat(int argc, char *argv[])
{
    struct stat f_stat;
    char *fn_goon, *fn_idx;
    ti_index_t *idx;

    const ti_conf_t *ti_conf;
    const char **names;
    int i, n;

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

    if ((idx = ti_index_load(fn_goon)) == 0) {
        fprintf(stderr,"error: failed to load the index file.\n");
        return -1;
    }

    ti_conf = ti_get_conf(idx);

    names = ti_seqname(idx, &n);
    for (i = 0; i < n; ++i) {
        printf("SEQUENCE_NAME  %s\n", names[i]);
    }
    printf("\n");

    printf("SEQUENCE_KEY   %s\n", ti_conf->sk);
    printf("START_KEY      %s\n", ti_conf->bk);
    printf("END_KEY        %s\n", ti_conf->ek ? ti_conf->ek : "");
    printf("ZERO_BASED     %s\n", ti_conf->zerobased ? "true" : "false");
    printf("RIGHT_OPEN     %s\n", ti_conf->zerobased ? "true" : "false");

    free(names);
    ti_index_destroy(idx);

    return 0;
}
