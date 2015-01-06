#include "goon.h"

void skip_ws(char **str)
{
    while (**str != '\0' && isspace(**str)) {
        *str += 1;
    }
}

Chrom_pos* get_chrom_pos_dirty(char *line,
                                      char *seq_key,
                                      char *start_key)
{
    Chrom_pos *chrom_pos;
    char *str_p, *str_p2, *endptr;
    int seen_chrom, seen_start;
    int seq_key_len = strlen(seq_key);
    int start_key_len = strlen(start_key);
    int line_len = strlen(line);

    chrom_pos = calloc(1, sizeof(Chrom_pos));
    seen_chrom = seen_start = 0;

    for (str_p = line; *str_p != '\0'; str_p += 1) {
        if (*str_p != '"') {
            continue;
        }
        // minimally "KEY":"."
        if (str_p - line + seq_key_len + 5 <= line_len 
            && *(str_p + seq_key_len + 1) == '"'
            && strncmp(str_p+1, seq_key, seq_key_len) == 0) {
            str_p += seq_key_len + 2;
            skip_ws(&str_p);
            if (*str_p != ':') {
                continue;
            }
            str_p += 1;
            skip_ws(&str_p);
            if (*str_p != '"') {
                fprintf(stderr, "[%s] sequence value is not a string: %s",
                        __func__, line);
                return NULL;
            }
            str_p += 1;
            for (str_p2 = str_p; *str_p2 != '\0' && *str_p2 != '"'; str_p2 += 1) {
                // empty
            }
            if (*str_p2 != '"') {
                fprintf(stderr, "[%s] sequence value is not a string: %s",
                        __func__, line);
                return NULL;
            }
            chrom_pos->chrom = strndup(str_p, str_p2 - str_p);
            str_p = str_p2 + 1;
            seen_chrom = 1;
          
        }
        // minimally "KEY":N
        else if (str_p - line + start_key_len + 3 <= line_len
            && *(str_p + start_key_len + 1) == '"'
            && strncmp(str_p+1, start_key, start_key_len) == 0) { 
            str_p += start_key_len + 2;
            skip_ws(&str_p);
            if (*str_p != ':') {
                continue;
            }            
            str_p += 1;
            chrom_pos->pos = strtol(str_p, &endptr, 10);
            if (endptr == str_p) {
                fprintf(stderr, "[%s] cannot extract position in line: %s",
                        __func__, line);
                return NULL;
            }
            str_p = endptr;
            seen_start = 1;
        }
        if (seen_chrom && seen_start) {
            break;
        }
    }

    if (!seen_chrom || !seen_start) {
        if (seen_chrom) {
            fprintf(stderr, "[%s] cannot extract position in line: %s",
                    __func__, line);
            free(chrom_pos->chrom);
        } else {
            fprintf(stderr, "[%s] cannot extract sequence in line: %s",
                    __func__, line);
        }
        free(chrom_pos);
        return NULL;
    }

    return chrom_pos;
}
