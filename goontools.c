#include "goontools.h"

void usage(char* prog)
{
    printf("\n");
    printf("Usage: %s <subcommand> <subcommand arguments>\n", prog);
    printf("\n");
    printf("subcommands:\n");
    printf("    index    index file\n");
    printf("    sort     sort file\n");
    printf("    view     view/slice file\n");
    printf("\n");
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
