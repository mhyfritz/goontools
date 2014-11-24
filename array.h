#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_DECLARE(TYPE) \
    typedef struct { \
        TYPE *elems; \
        unsigned long allocated, \
                      nextfree; \
    } Array##TYPE

#define ARRAY_INIT(A, TYPE, SIZE) \
    (A)->elems = (TYPE*)malloc(SIZE * sizeof(TYPE)); \
    (A)->allocated = SIZE; \
    (A)->nextfree = 0

#define ARRAY_PUSH(A, TYPE, VALUE) \
    if ((A)->allocated == (A)->nextfree) { \
        (A)->allocated *= 2; \
        (A)->elems = (TYPE*)realloc((A)->elems, (A)->allocated * sizeof(TYPE)); \
    } \
    (A)->elems[(A)->nextfree] = VALUE; \
    (A)->nextfree += 1

#define ARRAY_FREE(A) \
    if ((A)->elems != NULL) { \
        free((A)->elems); \
    }

#endif

