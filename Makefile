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

