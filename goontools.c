#include "goontools.h"

void usage(char* prog)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s <subcommand> <subcommand arguments>\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "subcommands:\n");
    fprintf(stderr, "    index    index file\n");
    fprintf(stderr, "    sort     sort file\n");
    fprintf(stderr, "    view     view/slice file\n");
    fprintf(stderr, "\n");
}

int dispatch(int argc, char *argv[])
{
    char *subcommands[] = {
        "index",
        "view",
        "sort",
        NULL // sentinel
    };
    Subcommand dispatch[] = {
        &goonindex,
        &goonview,
        &goonsort
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

    return dispatch[s-subcommands](argc-2, argv+2);
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
