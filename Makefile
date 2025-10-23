TARGET = main
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

$(BIN)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	$(BIN)/$(TARGET)

clean:
	rm -r $(OBJ) $(BIN)

format:
	@# Requires GNU Indent
	VERSION_CONTROL=never indent -kr --no-tabs $(SOURCES) $(HEADERS)

.PHONY: run clean format

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
GOLD_TARGETS = $(GOLD_TARGETS_ARC) # $(GOLD_TARGETS_TRC)

GOLD_GOLDENS_OUTDIR := $(GOLD_ROOT)/goldens
ifdef GOLD_OUT
	GOLD_GOLDENS_OUTDIR := $(GOLD_OUT)
endif
GOLD_GOLDENS_TARGETS := $(GOLD_TARGETS:$(GOLD_BIN)/%=$(GOLD_GOLDENS_OUTDIR)/%.golden)

$(GOLD_TARGETS_ARC) : $(GOLD_BIN)/%_arc:$(GOLD_SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(GOLD_CFLAGS) -DGC_ARC $< -o $@

gold : $(GOLD_TARGETS)

gold_clean :
	rm -rf $(GOLD_BIN)

$(GOLD_GOLDENS_TARGETS) : $(GOLD_GOLDENS_OUTDIR)/%.golden:$(GOLD_BIN)/% 
	@mkdir -p $(@D)
	$< | python3 script/normalize.py > $@

gold_generate : $(GOLD_GOLDENS_TARGETS)

gold_test:
	@python3 script/test_gold.py
