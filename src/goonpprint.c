#include "goonpprint.h"

#define PROG_ALIAS "goonpprint"
#define USAGE usage(PROG_ALIAS)

static void usage(char *prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <arguments> [goonfile]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -i/--indent     indentation per level "
                    "(default: %d)\n",
            GOON_PPRINT_DEFAULT_INDENT);
    fprintf(stderr, "    -n/--num        number of records to print\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

static void kson_dump_recur(const kson_node_t *p,
                            FILE *fp,
                            int indent,
                            int depth)
{
    long i;

    if (p->key) fprintf(fp, "\"%s\": ", p->key);
    if (p->type == KSON_TYPE_BRACKET || p->type == KSON_TYPE_BRACE) {
        fputc(p->type == KSON_TYPE_BRACKET? '[' : '{', fp);
        if (p->n) {
            fprintf(fp, "\n%*s", (depth+1) * indent, "");
            for (i = 0; i < (long)p->n; ++i) {
                if (i) {
                    fprintf(fp, ",\n%*s", (depth+1) * indent, "");
                }
                kson_dump_recur(p->v.child[i], fp, indent, depth + 1);
            }
            fprintf(fp, "\n%*s", depth * indent, "");
        }
        fputc(p->type == KSON_TYPE_BRACKET? ']' : '}', fp);
    } else {
        if (p->type != KSON_TYPE_NO_QUOTE)
            fputc(p->type == KSON_TYPE_SGL_QUOTE? '\'' : '"', fp);
        fputs(p->v.str, fp);
        if (p->type != KSON_TYPE_NO_QUOTE)
            fputc(p->type == KSON_TYPE_SGL_QUOTE? '\'' : '"', fp);
    } 
}

static void kson_dump(const kson_node_t *root, FILE *fp, int indent)
{
    kson_dump_recur(root, fp, indent, 0);
    fputc('\n', fp);
}

int goonpprint(int argc, char *argv[])
{
    struct option opts[] = {
        {"indent", required_argument, NULL, 'i'},
        {"num", required_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    struct stat f_stat;
    int c,
        indent = GOON_PPRINT_DEFAULT_INDENT,
        n = -1;
    FILE *f;
    Line line;
    kson_t *kson = NULL;

    init_line(&line);

    while ((c = getopt_long(argc,
                            argv,
                            "i:n:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': USAGE;
                      return -1;
            // TODO int check
            case 'i': indent = strtol(optarg, NULL, 0);
                      break;
            case 'n': n = strtol(optarg, NULL, 0);
                      if (n <= 0) {
                        fprintf(stderr, "error: num has to be "
                                        "positive integer\n");
                        return -1;
                      }
                      break;
            default: return -1;
        }
    }

    if (indent <= 0) {
        fprintf(stderr, "error: indent has to be positive integer\n");
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

    while (read_line(f, &line) > 0 && n != 0) {
        kson = kson_parse(line.elems);
        if (!kson) {
            fprintf(stderr, "cannot parse line: %s", line.elems);
            return -1;
        }
        kson_dump(kson->root, stdout, indent);
        if (n != -1) {
            n -= 1;
        }
    }

    if (optind != argc) {
        fclose(f);
    }

    free_line(&line);

    return 0;
} 
