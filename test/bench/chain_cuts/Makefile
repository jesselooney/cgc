# c setup
CC = gcc
CGC_LIB = ../../../include
HEADERS = $(wildcard $(CGC_LIB)/*)
CFLAGS += -I $(CGC_LIB)
CFLAGS += -Wno-incompatible-pointer-types

# cgc debug options
ifdef V
	CFLAGS += -DVERBOSITY=$(V)
endif
ifdef DBG
	CFLAGS += -g
endif

#################################
# BUILDING

BIN = bin
SOURCE = main.c
TARGETS = $(BIN)/ARC $(BIN)/TRC

.PHONY: default

default: $(TARGETS)

$(BIN)/ARC: $(SOURCE) $(HEADERS)
	@mkdir -p $(@D)
	@mkdir -p ./reports
	$(CC) $(CFLAGS) -DGC_ARC main.c -o $@

$(BIN)/TRC: $(SOURCE) $(HEADERS)
	@mkdir -p $(@D)
	@mkdir -p ./reports
	$(CC) $(CFLAGS) -DGC_TRC main.c -o $@

clean:
	rm -r bin

#################################
# RUNNING

ra: $(TARGETS)
	CGC_POOL_EXP=10 \
	CGC_HEAP_EXP=10 \
	taskset --cpu-list 1 \
	$(BIN)/ARC

rt: $(TARGETS)
	CGC_POOL_EXP=10 \
	CGC_HEAP_EXP=10 \
	taskset --cpu-list 1 \
	$(BIN)/TRC

ra_bench: $(TARGETS)
	CGC_OUTPATH=$(OUTFILE) \
	CGC_POOL_EXP=10 \
	CGC_HEAP_EXP=10 \
	taskset --cpu-list 1 \
	$(BIN)/ARC

rt_bench: $(TARGETS)
	CGC_OUTPATH=$(OUTFILE) \
	CGC_POOL_EXP=10 \
	CGC_HEAP_EXP=10 \
	taskset --cpu-list 1 \
	$(BIN)/TRC

#################################
# PYTHON

PYTHON = ./venv/bin/python
venv: 
	test -d venv || python3 -m venv venv
	$(PYTHON) -m pip install -r requirements.txt -q

# run + visualize, comparative (between arc and trc)
TIMESTAMP := $(shell date +"%y%m%d-%H%M%S")
rvc: $(TARGETS)
	ARC_OUT=reports/$(TIMESTAMP)_arc.csv ; \
	TRC_OUT=reports/$(TIMESTAMP)_trc.csv ; \
	$(MAKE) ra_bench OUTFILE="$$ARC_OUT" || exit 1 ; \
	$(MAKE) rt_bench OUTFILE="$$TRC_OUT" || exit 1; \
	$(PYTHON) vis.py $$ARC_OUT $$TRC_OUT

vc_recent: 
	FIRST=$$(ls reports | tail -n 2 | head -n 1); \
	SECOND=$$(ls reports | tail -n 1); \
	$(PYTHON) vis.py reports/$$FIRST reports/$$SECOND