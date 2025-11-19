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

run: $(BIN)
	./$(BIN)

clean:
	rm -r bin

.PHONY: run vis clean venv

#####

PYTHON = ./venv/bin/python

venv: 
	test -d venv || python3 -m venv venv
	$(PYTHON) -m pip install -r requirements.txt -q

rvis: $(BIN) venv
	./$(BIN)
	TEST=$$(find reports/*.csv | sort | tail -n 1); \
	$(PYTHON) vis.py $$TEST

vis: venv
	TEST=$$(find reports/*.csv | sort | tail -n 1); \
	$(PYTHON) vis.py $$TEST