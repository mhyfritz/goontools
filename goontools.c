#include "goontools.h"

void usage(char* prog)
{
    printf("\n");
    printf("Usage: %s <subcommand> <subcommand arguments>\n", prog);
    printf("\n");
    printf("subcommands:\n");
    printf("    index\tindex file\n");
    printf("    sort\tsort file\n");
    printf("    view\tview/slice file\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int ret;

    if (argc == 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "index") == 0) {
        ret = goonindex(argc-2, argv+2); 
    } else if (strcmp(argv[1], "view") == 0) {
        ret = goonview(argc-2, argv+2);
    } else if (strcmp(argv[1], "sort") == 0) {
        ret = goonsort(argc-2, argv+2);
    } else {
        usage(argv[0]);
        ret = -1;
    }

    return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
