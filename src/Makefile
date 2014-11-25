PROG = goontools

CC = gcc
CFLAGS = -g -Wall -O2 -fPIC 
CPPFLAGS = -D_FILE_OFFSET_BITS=64 -DBGZF_CACHE
LD = $(CC)
LDFLAGS = -lz

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
GOONSUBSRC = $(filter-out $(PROG).c,$(wildcard goon*.c))
PROTO = $(GOONSUBSRC:.c=.p)

.PHONY: all clean

all: $(PROTO) $(PROG)

$(PROG): $(OBJ)
	$(LD) -o $(PROG) $(OBJ) $(LDFLAGS)

$(PROTO): %.p: %.c
	perl get_c_proto.pl $< > $@

clean:
	$(RM) $(PROTO) $(OBJ) $(PROG)
