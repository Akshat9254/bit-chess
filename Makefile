CC = clang

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -g \
	-O0 -fsanitize=address,undefined \
	-MMD -MP -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:src/%.c=build/%.o)

# Objects without main for linking into tests
SRC_LIB = $(filter-out src/main.c,$(SRC))
OBJ_LIB = $(SRC_LIB:src/%.c=build/%.o)

# Tests
TEST_SRC = $(shell find tests -name '*.c' 2>/dev/null)
TEST_OBJ = $(TEST_SRC:tests/%.c=build/tests/%.o)
TEST_BIN = build/tests/run_tests

BIN = build/chess

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test objects
build/tests/%.o: tests/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link test binary
$(TEST_BIN): $(OBJ_LIB) $(TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

-include $(OBJ:.o=.d)
-include $(TEST_OBJ:.o=.d)

.PHONY: clean test run

clean:
	rm -rf build/

run: all
	./$(BIN)

# Run unit tests
test: $(TEST_BIN)
	./$(TEST_BIN)
