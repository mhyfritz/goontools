#include "goonextract.h"

#define PROG_ALIAS "goonextract"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s [arguments] <goonfile> "
                    "<key1> [key2 ... ]]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -d/--delim  field delimiter\n");
    fprintf(stderr, "    -n/--null   null value\n");
    fprintf(stderr, "    -h/--help   display help\n");
    fprintf(stderr, "\n");
}

int goonextract(int argc, char *argv[])
{
    struct option opts[] = {
        {"delim", required_argument, NULL, 'd'},
        {"null", required_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int c,
        i;
    char *d = DEFAULT_DELIMITER,
         *null = NULL,
         line[65536]; // FIXME
    FILE *fp;
    kson_t *kson = 0;
    const kson_node_t *p;
    
    if (argc < 2) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "d:n:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 'd': d = optarg;
                      break;
            case 'n': null = optarg;
                      break;
            default: return -1;
        }        
    }

    if (optind >= argc -1) {
        fprintf(stderr, "error: missing arguments\n");
        return -1;
    }

    if (strcmp(argv[optind], "-") == 0) {
        fp = stdin;
    } else {
        if ((fp = fopen(argv[optind], "r")) == NULL) {
            fprintf(stderr, "error: cannot open file %s\n", argv[optind]);
            return -1;
        }
    }

    while(fgets(line, 65536, fp) != NULL) {
        kson = kson_parse(line);
        if (kson) {
            for (i = optind + 1; i < argc; i += 1) {
                p = kson_by_key(kson->root, argv[i]);
                if (!p && !null) {
                    fprintf(stderr, "error: cannot find key %s\n", argv[i]);
                    return -1;
                }
                if (p && kson_is_internal(p)) {
                    fprintf(stderr, "error: compound value %s\n", argv[i]);
                    return -1;
                }
                if (i > optind + 1) {
                    printf("%s", d);
                }
                printf("%s", p ? p->v.str : null);
            }
            printf("\n");
        } else {
            fprintf(stderr, "cannot parse line: %s", line);
            return -1;
        }
    }

    fclose(fp);
    kson_destroy(kson);

    return 0;
}
