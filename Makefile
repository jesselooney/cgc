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

GOLD_ROOT = test/gold
GOLD_SRC = $(GOLD_ROOT)/src
GOLD_OBJ = $(GOLD_ROOT)/obj
GOLD_BIN = $(GOLD_ROOT)/bin

GOLD_SOURCES := $(wildcard $(GOLD_SRC)/*.c)
GOLD_OBJECTS := $(GOLD_SOURCES:$(GOLD_SRC)/%.c=$(GOLD_OBJ)/%.o)
GOLD_TARGETS := $(GOLD_SOURCES:$(GOLD_SRC)/%.c=$(GOLD_BIN)/%)

$(GOLD_TARGETS) : $(GOLD_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(GOLD_OBJECTS) $(LDLIBS) -o $@

$(GOLD_OBJECTS) : $(GOLD_OBJ)/%.o : $(GOLD_SRC)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

gold : $(GOLD_TARGETS)

#gold_generate

#gold_run
