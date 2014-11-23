PROG = goontools

CC = gcc
CFLAGS = -g -Wall -O2 -fPIC 
CPPFLAGS = -D_FILE_OFFSET_BITS=64 -DBGZF_CACHE
LD = $(CC)
LDFLAGS = -lz

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJ)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJ)

clean:
	$(RM) $(OBJ) $(PROG)
