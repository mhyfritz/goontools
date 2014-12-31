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
    fprintf(stderr, "    -t/--threads    number of threads (default=1)\n");
    fprintf(stderr, "    -m/--maxmem     maximal amount of memory per thread "
                                         "(suffixes K/M/G recognized; "
                                         "default=%luM)\n", MAX_MEM_MB_DEFAULT);
    fprintf(stderr, "    -p/--prefix     temp file prefix (default='%s')\n",
                                         TEMP_FILE_PREFIX_DEFAULT);
    fprintf(stderr, "    -o/--outfile    name of output bgzip file\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

static void gn_conf_init(Gn_sort_conf *conf)
{
    conf->max_mem = MAX_MEM_MB_DEFAULT << 20;
    conf->seq_key = conf->start_key = conf->prefix = conf->outfile = NULL;
    conf->n_threads = 1;
}

int goonsort(int argc, char *argv[])
{
    struct option opts[] = {
        {"seqkey", required_argument, NULL, 's'},
        {"startkey", required_argument, NULL, 'b'},
        {"threads", required_argument, NULL, 't'},
        {"mem", required_argument, NULL, 'm'},
        {"prefix", required_argument, NULL, 'p'},
        {"outfile", required_argument, NULL, 'o'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c;
    FILE *f;
    Gn_sort_conf conf;
    
    gn_conf_init(&conf);

    if (argc == 1) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "s:b:t:m:p:o:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 's': conf.seq_key = optarg;
                      break;
            case 'b': conf.start_key = optarg;
                      break;
            case 't': conf.n_threads = atoi(optarg);
                      break;
            case 'm': {
                          char *q;
                          conf.max_mem = strtol(optarg, &q, 0);
                          if (*q == 'k' || *q == 'K') {
                              conf.max_mem <<= 10;
                          } else if (*q == 'm' || *q == 'M') {
                              conf.max_mem <<= 20;
                          } else if (*q == 'g' || *q == 'G') {
                              conf.max_mem <<= 30;
                          }
                          break;
                      }
            case 'p': conf.prefix = optarg;
                      break;
            case 'o': conf.outfile = optarg;
                      break;
            default: return -1;
        }
    }

    if (conf.seq_key == NULL || conf.start_key == NULL) {
        fprintf(stderr, "error: missing mandatory argument\n");
        return -1;
    }

    if (conf.n_threads < 1) {
        fprintf(stderr, "error: `threads` has to be greater than zero\n");
        return -1;
    }

    if (conf.prefix == NULL) {
        conf.prefix = TEMP_FILE_PREFIX_DEFAULT;
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

    if (goon_sort_core(f, &conf) != 0) {
        return -1;
    }

    if (optind != argc) {
        fclose(f);
    }

    return 0;
} 
