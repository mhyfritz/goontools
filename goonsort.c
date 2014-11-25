#include "goonsort.h"

#define PROG_ALIAS "goonsort"
#define USAGE usage(PROG_ALIAS)

ARRAY_DECLARE(Gn_sort_record_t);

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> [goonfile]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -s/--seqkey     sequence key (mandatory)\n");
    fprintf(stderr, "    -b/--startkey   start position key (mandatory)\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

static inline int sort_gn_record_lt(const Gn_sort_record_t a,
                                    const Gn_sort_record_t b)
{
    int ret_str = strcmp(a.chrom, b.chrom);
    int ret_int = a.pos - b.pos;
    if (ret_str < 0 || (ret_str == 0 && ret_int < 0)) {
        return 1;
    }
    return 0;
}

KSORT_INIT(sort, Gn_sort_record_t, sort_gn_record_lt);

static Gn_sort_record_p construct_gn_sort_record(char *line,
                                                 char *seq_needle,
                                                 char *start_needle)
{
    Gn_sort_record_p r = malloc(sizeof(Gn_sort_record_t));
    char *str_p, *str_p2;
    int seen_chrom, seen_start;

    seen_chrom = seen_start = 0;
    r->json = strdup(line);

    for (str_p = line; *str_p != '\0'; str_p += 1) {
        if (*str_p != '"') {
            continue;
        }
        if (strncmp(str_p, seq_needle, strlen(seq_needle)) == 0) {
            str_p += strlen(seq_needle);
            skip_ws(&str_p);
            assert(*str_p == '"');
            str_p += 1;
            for (str_p2 = str_p; *str_p2 != '\0' && *str_p2 != '"'; str_p2 += 1) {
                // empty
            }
            assert(*str_p2 == '"');
            r->chrom = strndup(str_p, str_p2 - str_p);
            str_p = str_p2 + 1;
            seen_chrom = 1;
        } else if (strncmp(str_p, start_needle, strlen(start_needle)) == 0) {
            str_p += strlen(start_needle);
            r->pos = strtol(str_p, &str_p, 10);
            seen_start = 1;
        }
        if (seen_chrom && seen_start) {
            break;
        }
    } 

    if (!seen_chrom || !seen_start) {
        return NULL;
    }

    return r;
}

int goonsort(int argc, char *argv[])
{
    struct option opts[] = {
        {"seqkey", required_argument, NULL, 's'},
        {"startkey", required_argument, NULL, 'b'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c;
    char *seq_key = NULL,
         *start_key = NULL,
         *seq_needle = NULL,
         *start_needle = NULL,
         line[16384]; // FIXME Hello, overflow!
    FILE *f;
    ArrayGn_sort_record_t gn_records;
    Gn_sort_record_p r;

    ARRAY_INIT(&gn_records, Gn_sort_record_t, 16384);

    if (argc == 1) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "s:b:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 's': seq_key = optarg;
                      break;
            case 'b': start_key = optarg;
                      break;
            default: return -1;
        }
    }

    if (seq_key == NULL || start_key == NULL) {
        fprintf(stderr, "error: missing mandatory argument\n");
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

    seq_needle = malloc(strlen(seq_key) + 4);
    start_needle = malloc(strlen(start_key) + 4);

    sprintf(seq_needle, "\"%s\":", seq_key);
    sprintf(start_needle, "\"%s\":", start_key);

    while (fgets(line, sizeof(line), f)) {
        r = construct_gn_sort_record(line, seq_needle, start_needle);
        assert(r != NULL);
        ARRAY_PUSH(&gn_records, Gn_sort_record_t, *r);
        free(r);
    }

    fclose(f);

    ks_mergesort(sort, gn_records.nextfree, gn_records.elems, NULL);

    for (r = gn_records.elems; r - gn_records.elems < gn_records.nextfree; r += 1) {
        printf("%s", r->json);
        free(r->json);
        free(r->chrom);
    }

    free(seq_needle);
    free(start_needle);

    free(gn_records.elems);

    return 0;
} 
