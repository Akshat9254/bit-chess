CC = clang

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -g \
	-O0 -fsanitize=address,undefined \
	-MMD -MP -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:src/%.c=build/%.o)

BIN = build/chess

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJ:.o=.d)

clean:
	rm -rf build/

run: all
	./$(BIN)
