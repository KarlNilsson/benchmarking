CC=gcc
CFLAGS=-I.

ifeq ($(PRINTOUT),1)
	CFLAGS+= -D PRINTOUT
endif

ODIR=include

_OBJ = uds_1-1.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

BINS=uds_1-1

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

uds_1-1: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINS)
