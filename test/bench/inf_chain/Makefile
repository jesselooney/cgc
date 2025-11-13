CC = gcc
CGC_LIB = ../../../include
HEADERS = $(wildcard $(CGC_LIB)/*)
CFLAGS += -I $(CGC_LIB) 
CFLAGS += -Wno-incompatible-pointer-types
ifdef V
	CFLAGS += -DVERBOSITY=$(V)
endif
ifdef DBG
	CFLAGS += -g
endif

bin/arc: main.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DGC_ARC main.c -o $@

bin/trc: main.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DGC_TRC main.c -o $@

run: bin/arc
	./bin/arc

runt : bin/trc
	./bin/trc


clean:
	rm -r bin

.PHONY: clean run arc trc