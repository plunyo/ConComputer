CC = gcc
DEBUG ?= 1
CFLAGS = -g -Wall -Werror -Isrc

ifeq ($(DEBUG),1)
    CFLAGS += -DDEBUG=1
endif

LDFLAGS = -lraylib -lm

SRC = $(shell find src -name "*.c")
OBJ_DIR = build
OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) compile_commands.json

run: $(TARGET)
	./$(TARGET)

compile_commands: clean
	bear -- make

compile_commands_debug: clean
	bear -- make DEBUG=1

compile_debug: clean
	$(MAKE) DEBUG=1 all

.PHONY: all clean run compile_commands compile_commands_debug compile_debug
