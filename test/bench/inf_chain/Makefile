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

.PHONY: run vis clean venv rrun rvis vis both

$(BIN): main.c $(HEADERS)
	@mkdir -p $(@D)
	@mkdir -p ./reports
	$(CC) $(CFLAGS) -DGC_$(GC) main.c -o $@

both:
	$(MAKE) GC=ARC
	$(MAKE) GC=TRC

# run (no monitoring)
run: $(BIN)
	./$(BIN)

clean:
	rm -r bin

#####

PYTHON = ./venv/bin/python
TIMESTAMP := $(shell date +"%y%m%d-%H%M%S")
OUTFILE := reports/$(TIMESTAMP).csv

rrun: $(BIN)
	CGC_OUTPATH=$(OUTFILE) \
	./$(BIN)

venv: 
	test -d venv || python3 -m venv venv
	$(PYTHON) -m pip install -r requirements.txt -q

# run and visualize
rvis: $(BIN) venv
	CGC_OUTPATH=$(OUTFILE) \
	CGC_POOL_EXP=10 \
	CGC_HEAP_EXP=10 \
	taskset --cpu-list 1 \
	./$(BIN)
	$(PYTHON) vis.py $(OUTFILE)

# most recent
vis: venv
	TEST=$$(find reports/*.csv | sort | tail -n 1); \
	$(PYTHON) vis.py $$TEST
