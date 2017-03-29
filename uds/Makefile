CC=gcc
CFLAGS=-I.
THR_CFLAGS=-lpthread

ifeq ($(PRINTOUT),1)
	CFLAGS+= -D PRINTOUT
endif

ODIR=include

_OBJ = uds_1-1.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_THR_OBJ = uds_1-1.o
THR_OBJ = $(patsubst %,$(ODIR)/%,$(_THR_OBJ))

BINS=uds_1-1

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ): $(_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(THR_OBJ): $(_THR_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(THR_CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINS)
