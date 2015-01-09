#include "goontools.h"

void usage(char* prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <subcommand> <subcommand arguments>\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "subcommands:\n");
    fprintf(stderr, "    index      index file\n");
    fprintf(stderr, "    sort       sort file\n");
    fprintf(stderr, "    view       view file / subset lines\n");
    fprintf(stderr, "    idxstat    print index information\n");
    fprintf(stderr, "    extract    extract record values (experimental)\n");
    fprintf(stderr, "    pprint     pretty-print records\n");
    fprintf(stderr, "\n");
}

int dispatch(int argc, char *argv[])
{
    char *subcommands[] = {
        "index",
        "view",
        "sort",
        "idxstat",
        "extract",
        "pprint",
        NULL // sentinel
    };
    Subcommand dispatch[] = {
        &goonindex,
        &goonview,
        &goonsort,
        &goonidxstat,
        &goonextract,
        &goonpprint
    };
    char **s;

    for (s = subcommands; *s != NULL; s += 1) {
        if (strcmp(argv[1], *s) == 0) {
            break;
        }
    }

    if (*s == NULL) {
        usage(argv[0]);
        return -1;
    }

    return dispatch[s-subcommands](argc-1, argv+1);
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (dispatch(argc, argv) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
