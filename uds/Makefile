CC=gcc
CFLAGS=-I.

ifeq ($(DEBUG),1)
	CFLAGS+= -D PRINTOUT
endif

ODIR=include

_OBJ = uds_1-1.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
BINS=uds_1-1

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINS): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINS)
