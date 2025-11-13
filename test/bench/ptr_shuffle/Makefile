CC = gcc
CGC_LIB = ../../../include
CFLAGS += -I $(CGC_LIB) 
CFLAGS += -Wno-incompatible-pointer-types
ifdef V
	CFLAGS += -DVERBOSITY=$(V)
endif
ifdef DBG
	CFLAGS += -g
endif

bin/main: main.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DGC_TRC main.c -o $@

clean:
	rm -r bin

run: bin/main
	bin/main

.PHONY: clean run
