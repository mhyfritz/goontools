// modified from bam_sort.c (samtools-0.1.19), written by Heng Li

#include "goonsort.h"

static inline int sort_gn_record_lt(const Gn_sort_record_p a,
                                    const Gn_sort_record_p b)
{
    int ret_str = strcmp(a->chrom_pos->chrom, b->chrom_pos->chrom);
    int ret_int = a->chrom_pos->pos - b->chrom_pos->pos;
    if (ret_str < 0 || (ret_str == 0 && ret_int < 0)) {
        return 1;
    }
    return 0;
}

static Chrom_pos* get_chrom_pos(char *line,
                                char *seq_needle,
                                char *start_needle)
{
    Chrom_pos *chrom_pos;
    char *str_p, *str_p2;
    int seen_chrom, seen_start;

    chrom_pos = calloc(1, sizeof(Chrom_pos));
    seen_chrom = seen_start = 0;

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
            chrom_pos->chrom = strndup(str_p, str_p2 - str_p);
            str_p = str_p2 + 1;
            seen_chrom = 1;
        } else if (strncmp(str_p, start_needle, strlen(start_needle)) == 0) {
            str_p += strlen(start_needle);
            chrom_pos->pos = strtol(str_p, &str_p, 10);
            seen_start = 1;
        }
        if (seen_chrom && seen_start) {
            break;
        }
    }

    if (!seen_chrom || !seen_start) {
        if (seen_chrom) {
            free(chrom_pos->chrom);
        }
        free(chrom_pos);
        return NULL;
    }

    return chrom_pos;
}

static int construct_gn_sort_record(Gn_sort_record_p *r,
                                    char *line,
                                    char *seq_needle,
                                    char *start_needle)
{
    if (((*r)->chrom_pos = get_chrom_pos(line,
                                         seq_needle,
                                         start_needle)) == NULL) {
        return -1;
    }
    (*r)->json = strdup(line);

    return 0;
}

KSORT_INIT(sort, Gn_sort_record_p, sort_gn_record_lt);

static void write_buffer(const char *fn,
                         const char *mode,
                         size_t l,
                         Gn_sort_record_p *buf)
{
    BGZF *fp;
    size_t i;

    if ((fp = bgzf_open(fn, mode)) == NULL) {
        return;
    }

    for (i = 0; i < l; ++i) {
        bgzf_write(fp, (void *)buf[i]->json, strlen(buf[i]->json));
    }

    bgzf_close(fp);
} 

static char* construct_tmp_fname(const char *prefix, int i)
{
    char *name;

    name = (char*)calloc(strlen(prefix) + 14, 1);
    sprintf(name, "%s.%04d.goon.gz", prefix, i);

    return name;
}

typedef struct {
    size_t buf_len;
    const char *prefix;
    Gn_sort_record_p *buf;
    int index;
} worker_t;

static void *worker(void *data)
{
    worker_t *w = (worker_t*)data;
    char *name;
    ks_mergesort(sort, w->buf_len, w->buf, 0);
    name = construct_tmp_fname(w->prefix, w->index);
    write_buffer(name, "w1", w->buf_len, w->buf);
    free(name);
    return 0;
}


static int sort_blocks(int n_files,
                       size_t k,
                       Gn_sort_record_p *buf,
                       const char *prefix,
                       int n_threads)
{
    int i;
    size_t rest;
    Gn_sort_record_p *b;
    pthread_t *tid;
    pthread_attr_t attr;
    worker_t *w;

    if (k == 0) {
        return n_files;
    }
    
    // use a single thread if we only sort a small batch of records
    if (k < n_threads * 64) {
        n_threads = 1;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    w = calloc(n_threads, sizeof(worker_t));
    tid = calloc(n_threads, sizeof(pthread_t));
    b = buf;
    rest = k;
    
    for (i = 0; i < n_threads; i += 1) {
        w[i].buf_len = rest / (n_threads - i);
        w[i].buf = b;
        w[i].prefix = prefix;
        w[i].index = n_files + i;
        b += w[i].buf_len;
        rest -= w[i].buf_len;
        pthread_create(&tid[i], &attr, worker, &w[i]);
    }

    for (i = 0; i < n_threads; i += 1) {
        pthread_join(tid[i], 0);
    }

    free(tid);
    free(w);

    return n_files + n_threads;
}

static inline int merge_gn_record_lt(const Gn_merge_record_t a,
                                     const Gn_merge_record_t b)
{
    if (a.chrom_pos->pos == HEAP_EMPTY && b.chrom_pos->pos != HEAP_EMPTY) {
        return 1;
    } else if (b.chrom_pos->pos == HEAP_EMPTY) {
        return 0;
    }

    int ret_str = strcmp(a.chrom_pos->chrom, b.chrom_pos->chrom);

    if (ret_str > 0) {
        return 1;
    }

    if (ret_str == 0) {
        if (a.chrom_pos->pos > b.chrom_pos->pos) {
            return 1;
        }
        if (a.chrom_pos->pos == b.chrom_pos->pos && a.i > b.i) {
            return 1;
        }
    }

    if (a.i == b.i && a.idx > b.idx) {
        return 1;
    } 

    return 0;
}

static int construct_gn_merge_record(Gn_merge_record_p r,
                                     int i,
                                     size_t idx,
                                     char *line,
                                     char *seq_needle,
                                     char *start_needle)
{
    if ((r->chrom_pos = get_chrom_pos(line,
                                      seq_needle,
                                      start_needle)) == NULL) {
        return -1;
    }
    r->json = strdup(line);
    r->i = i;
    r->idx = idx;

    return 0;
}

KSORT_INIT(heap, Gn_merge_record_t, merge_gn_record_lt);

int goon_merge_core(int n,
                    char * const *fns,
                    char *seq_needle,
                    char *start_needle)
{
    BGZF **fps;
    int i;
    Gn_merge_record_p heap;
    size_t idx;
    kstring_t *str;

    fps = calloc(n, sizeof(BGZF*));

    for (i = 0; i < n; i += 1) {
        fps[i] = bgzf_open(fns[i], "r");
        if (fps[i] == NULL) {
            fprintf(stderr, "[goon_merge_core] error: cannot open file '%s'\n",
                    fns[i]);
            return -1;
        }
    }

    heap = (Gn_merge_record_p)calloc(n, sizeof(Gn_merge_record_t));
    str = calloc(1, sizeof(kstring_t));

    idx = 0;
    for (i = 0; i < n; i += 1) {
        bgzf_getline(fps[i], '\n', str);
        construct_gn_merge_record(&heap[i],
                                  i,
                                  idx,
                                  str->s,
                                  seq_needle,
                                  start_needle);
        idx += 1;
    }

    ks_heapmake(heap, n, heap);
    
    while (heap->chrom_pos->pos != HEAP_EMPTY) {
        printf("%s\n", heap->json);
        if (bgzf_getline(fps[heap->i], '\n', str) >= 0) {
            free(heap->chrom_pos->chrom);
            construct_gn_merge_record(heap,
                                      heap->i,
                                      idx,
                                      str->s,
                                      seq_needle,
                                      start_needle);
            idx += 1;
        } else {
            heap->chrom_pos->pos = HEAP_EMPTY;
            free(heap->chrom_pos->chrom);
        }
        ks_heapadjust(heap, 0, n, heap);
    }

    for (i = 0; i < n; i += 1) {
        bgzf_close(fps[i]);
    }

    free(fps);
    free(heap);
    free(str->s);
    free(str);

    return 0;
}

int goon_sort_core(FILE *f, Gn_sort_conf *conf)
{
    char *seq_needle,
         *start_needle;
    size_t mem, max_k, k, max_mem;
    int n_files;
    Gn_sort_record_p b, *buf = NULL;
    int i;
    Line line;

    init_line(&line);

    max_k = k = mem = 0;
    max_mem = conf->max_mem * conf->n_threads;
    n_files = 0;

    seq_needle = malloc(strlen(conf->seq_key) + 4);
    start_needle = malloc(strlen(conf->start_key) + 4);

    // FIXME need proper parser
    sprintf(seq_needle, "\"%s\":", conf->seq_key);
    sprintf(start_needle, "\"%s\":", conf->start_key);

    for (;;) {
        if (k == max_k) {
            size_t old_max = max_k;
            max_k = max_k ? max_k << 1 : 0x10000;
            buf = realloc(buf, max_k * sizeof(Gn_sort_record_p));
            memset(buf + old_max, 0, sizeof(Gn_sort_record_p) * (max_k - old_max));
        }

        if (readline(f, &line) == 0) {
            break;
        } 

        store_line_char(&line, '\n');

        if (buf[k] == 0) {
            buf[k] = (Gn_sort_record_p)calloc(1, sizeof(Gn_sort_record_t));
        }
        
        b = buf[k];

        if (construct_gn_sort_record(&b,
                                     line.elems,
                                     seq_needle,
                                     start_needle) != 0) {
            return -1;
        }

        mem += sizeof(Gn_sort_record_t)
               + strlen(b->json) + 1
               + strlen(b->chrom_pos->chrom) + 1
               + sizeof(void*) + sizeof(void*); // pointer arrays 
        //fprintf(stderr, "k: %lu, mem: %lu\n", k, mem);
        k += 1;

        if (mem >= max_mem) {
            //fprintf(stderr, "mem=%lu, max_mem=%lu\n", mem, max_mem);
            n_files = sort_blocks(n_files,
                                  k, 
                                  buf, 
                                  conf->prefix, 
                                  conf->n_threads);
            mem = k = 0;
        }
    }

    if (n_files == 0) {
        ks_mergesort(sort, k, buf, 0);
        for (i = 0; i < k; i++) {
            printf("%s", buf[i]->json);
        }
    } else {
        char **fns;
        // sort & spill last chunk
        n_files = sort_blocks(n_files,
                              k,
                              buf,
                              conf->prefix,
                              conf->n_threads);
        fns = (char **)calloc(n_files, sizeof(char*));
        fprintf(stderr, "[goon_sort_core] merging %d files\n", n_files);
        for (i = 0; i < n_files; i += 1) {
            fns[i] = construct_tmp_fname(conf->prefix, i);
        }

        if (goon_merge_core(n_files, fns, seq_needle, start_needle) != 0) {
            return -1;
        }

        for (i = 0; i < n_files; i += 1) {
            unlink(fns[i]);
            free(fns[i]);
        }
        free(fns);
    }

    free(seq_needle);
    free(start_needle);
    free_line(&line);

    return 0;
}
