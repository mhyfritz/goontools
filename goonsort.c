#include "goonsort.h"

static void usage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "GOON sort\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "arguments:\n");
    fprintf(stderr, "    -s/--seqkey     sequence key (mandatory)\n");
    fprintf(stderr, "    -b/--startkey   start position key (mandatory)\n");
    fprintf(stderr, "    -h/--help       display help\n");
    fprintf(stderr, "\n");
}

int goonsort(int argc, char *argv[])
{
    struct option opts[] = {
        {"seqkey", required_argument, NULL, 's'},
        {"startkey", required_argument, NULL, 'b'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int c;
    char *seq_key = NULL, *start_key = NULL;

    if (argc == 1) {
        usage();
        return -1;
    }

    while ((c = getopt_long(argc,
                            argv,
                            "s:b:h",
                            opts,
                            NULL)) != -1) {
        switch (c) {
            case 'h': usage();
                      return -1;
            case 's': seq_key = optarg;
                      break;
            case 'b': start_key = optarg;
                      break;
            default: return -1;
        }
    }

    if (seq_key == NULL || start_key == NULL) {
        usage();
        return -1;
    }

    return 0;
} 
