CC = clang

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -g \
	-O0 -fsanitize=address,undefined \
	-MMD -MP -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:src/%.c=build/%.o)

# Objects without main for linking into tests
SRC_LIB = $(filter-out src/main.c,$(SRC))
OBJ_LIB = $(SRC_LIB:src/%.c=build/%.o)

# --- Test Logic Changes Start Here ---

# Find all test source files
TEST_SRC = $(shell find tests -name '*.c' 2>/dev/null)

# Define individual binaries for each test (e.g., build/tests/bitboard_test)
TEST_BINS = $(TEST_SRC:tests/%.c=build/tests/%)

# --- End of Changes ---

BIN = build/chess

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule: How to build each individual test binary
# It links the specific test .c file with your library objects
build/tests/%: tests/%.c $(OBJ_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

-include $(OBJ:.o=.d)

.PHONY: clean test run

clean:
	rm -rf build/

run: all
	./$(BIN)

# Run all unit tests sequentially
test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "Executing $$test..."; \
		./$$test || exit 1; \
	done
	@echo "All tests passed successfully!"

greet:
	@echo "Hello, $(NAME)"
