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
	@mkdir -p ./reports
	$(CC) $(CFLAGS) -DGC_$(GC) main.c -o $@

# run (no monitoring)
run: $(BIN)
	./$(BIN)

clean:
	rm -r bin

.PHONY: run vis clean venv rvis vis

#####

PYTHON = ./venv/bin/python
TIMESTAMP := $(shell date +"%y%m%d-%H%M%S")
OUTFILE := reports/$(TIMESTAMP).csv

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