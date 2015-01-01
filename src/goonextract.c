#include "goonextract.h"

#define PROG_ALIAS "goonextract"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s [arguments] [goonfile]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -p/--path    extraction path\n"
                    "                 (can be passed multiple time)\n");
    fprintf(stderr, "    -d/--delim   field delimiter\n");
    fprintf(stderr, "    -n/--null    null value\n");
    fprintf(stderr, "    -h/--help    display help\n");
    fprintf(stderr, "\n");
}

int goonextract(int argc, char *argv[])
{
    struct option opts[] = {
        {"path", required_argument, NULL, 'p'},
        {"delim", required_argument, NULL, 'd'},
        {"null", required_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int c,
        i,
        j;
    char *d = DEFAULT_DELIMITER,
         *null = NULL,
         line[65536]; // FIXME
    FILE *fp;
    kson_t *kson = 0;
    const kson_node_t *p;
    ArrayPath paths;
    Path path;
    
    ARRAY_INIT(&paths, Path, 64);

    if (argc == 1) {
        USAGE;
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "p:d:n:e",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            case 'p': if (optind == argc) {
                          fprintf(stderr, "error: missing file name\n"); 
                          return -1;
                      }
                      path.start = optind - 1;
                      for (i = path.start;
                           i < argc - 1 && argv[i][0] != '-';
                           i += 1) {
                        // empty
                      }
                      path.end = i - 1;
                      ARRAY_PUSH(&paths, Path, path);
                      optind = i;
                      break;
            case 'd': if (optind == argc) {
                          fprintf(stderr, "error: missing file name\n");
                          return -1;
                      }
                      d = optarg;
                      break;
            case 'n': null = optarg;
                      break;
            default: return -1;
        }        
    }

    if (paths.nextfree == 0) {
        fprintf(stderr, "error: missing path(s)\n");
        return -1;

    }

    if (strcmp(argv[argc-1], "-") == 0) {
        fp = stdin;
    } else {
        if ((fp = fopen(argv[argc-1], "r")) == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[argc-1]);
            return -1;
        }
    }

    while(fgets(line, 65536, fp) != NULL) {
        kson = kson_parse(line);
        if (kson) {
            for (i = 0; i < paths.nextfree; i += 1) {
                p = kson->root;
                path = paths.elems[i];
                for (j = path.start; j <= path.end; j += 1) {
                    if (p->type == KSON_TYPE_BRACKET) {
                        p = kson_by_index(p, atoi(argv[j]));
                    } else if (p->type == KSON_TYPE_BRACE) {
                        p = kson_by_key(p, argv[j]);
                    } 
                    if (!p && j < path.end){
                        // FIXME print path
                        fprintf(stderr, "error cannot resolve path\n");
                        return -1;
                    }
                }
                if (!p && !null) {
                    fprintf(stderr, "error: cannot find '%s'\n",
                            argv[path.end]);
                    return -1;
                }
                if (p && kson_is_internal(p)) {
                    fprintf(stderr, "error: compound value '%s'\n",
                            argv[path.end]);
                    return -1;
                }
                printf("%s%s", i > 0 ? d : "", p ? p->v.str : null);
                }
            printf("\n");
        } else {
            fprintf(stderr, "cannot parse line: %s", line);
            return -1;
        }
    }

    fclose(fp);
    kson_destroy(kson);
    ARRAY_FREE(&paths);

    return 0;
}
