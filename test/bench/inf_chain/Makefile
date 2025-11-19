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

GC ?= ARC
BIN = bin/$(GC)

$(BIN): main.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DGC_$(GC) main.c -o $@

run: $(BIN)
	./$(BIN)

vis: $(BIN)
	./$(BIN)
	@TEST=$$(find reports/*.csv | sort | tail -n 1); \
	python3 vis.py $$TEST

clean:
	rm -r bin

.PHONY: clean run arc trc