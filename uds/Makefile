CC=gcc
CFLAGS=-I.
THR_FLAGS=-lpthread

ifeq ($(PRINTOUT),1)
	CFLAGS+= -D PRINTOUT
endif

ODIR=include

_OBJ = uds_1-1.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_THR_OBJ = uds_1-n.o
THR_OBJ = $(patsubst %,$(ODIR)/%,$(_THR_OBJ))


BINS=uds_1-1 uds_1-n

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(BINS)

uds_1-1: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

uds_1-n: $(THR_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(THR_FLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINS)
