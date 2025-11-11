SRC = src
INCLUDE = include
OBJ = obj
BIN = bin

CC = gcc
CFLAGS += -I $(INCLUDE)
CFLAGS += -Wno-incompatible-pointer-types
ifdef V
	CFLAGS += -DVERBOSITY=$(V)
endif
ifdef DBG
	CFLAGS += -g
endif

LINKER = gcc

SOURCES := $(wildcard $(SRC)/*.c)
HEADERS := $(wildcard $(INCLUDE)/*.h)
OBJECTS := $(SOURCES:$(SRC)/%.c=$(OBJ)/%.o)
TARGETS := $(SOURCES:$(SRC)/%.c=$(BIN)/%)

DEFAULT_TARGET := main

default: $(TARGETS)

$(TARGETS): $(BIN)/%:$(SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

run: $(BIN)/$(DEFAULT_TARGET)
	$(BIN)/$(DEFAULT_TARGET)

run_norm : $(BIN)/$(DEFAULT_TARGET)
	$(BIN)/$(DEFAULT_TARGET) | python3 script/normalize.py

clean:
	rm -r $(OBJ) $(BIN)

format:
	@# Requires GNU Indent
	VERSION_CONTROL=never indent -kr --no-tabs $(SOURCES) $(HEADERS)

.PHONY: default run clean format

# ===============================================
# GOLD
# ===============================================

GOLD_CFLAGS += -I $(INCLUDE)
GOLD_CFLAGS += -Wno-incompatible-pointer-types
GOLD_CFLAGS += -DVERBOSITY=3

GOLD_ROOT = test/gold
GOLD_SRC = $(GOLD_ROOT)/src
GOLD_BIN = $(GOLD_ROOT)/bin

GOLD_SOURCES := $(wildcard $(GOLD_SRC)/*.c)
GOLD_OBJECTS := $(GOLD_SOURCES:$(GOLD_SRC)/%.c=$(GOLD_OBJ)/%.o)
GOLD_TARGETS_ARC := $(GOLD_SOURCES:$(GOLD_SRC)/%.c=$(GOLD_BIN)/%_arc)
GOLD_TARGETS_TRC := $(GOLD_SOURCES:$(GOLD_SRC)/%.c=$(GOLD_BIN)/%_trc)
GOLD_TARGETS = $(GOLD_TARGETS_TRC) $(GOLD_TARGETS_ARC) 

GOLD_GOLDENS_OUTDIR := $(GOLD_ROOT)/goldens
ifdef GOLD_OUT
	GOLD_GOLDENS_OUTDIR := $(GOLD_OUT)
endif
GOLD_GOLDENS_TARGETS := $(GOLD_TARGETS:$(GOLD_BIN)/%=$(GOLD_GOLDENS_OUTDIR)/%.golden)

$(GOLD_TARGETS_ARC) : $(GOLD_BIN)/%_arc:$(GOLD_SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(GOLD_CFLAGS) -DGC_ARC $< -o $@

$(GOLD_TARGETS_TRC) : $(GOLD_BIN)/%_trc:$(GOLD_SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(GOLD_CFLAGS) -DGC_TRC $< -o $@

gold : $(GOLD_TARGETS)

gold_clean :
	rm -rf $(GOLD_BIN)

$(GOLD_GOLDENS_TARGETS) : $(GOLD_GOLDENS_OUTDIR)/%.golden:$(GOLD_BIN)/% 
	@mkdir -p $(@D)
	$< | python3 script/normalize.py --gold > $@

gold_generate : $(GOLD_GOLDENS_TARGETS)

gold_test : $(GOLD_TARGETS)
	@python3 script/test_gold.py
