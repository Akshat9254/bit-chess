# ==============================================================================
# Makefile for bit-chess — Bitboard Chess Engine
# Optimized for CLion / Static Analysis Visibility
# ==============================================================================

TARGET      := bit-chess
SRC_DIR     := src
INC_DIR     := include
TEST_DIR    := tests
TOOLS_DIR   := tools
PERFT_DIR   := perft
BUILD_DIR   := build

BIN         := $(BUILD_DIR)/$(TARGET)

# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
CC      := clang
RM      := rm -rf

# ------------------------------------------------------------------------------
# Source / Object Discovery
# ------------------------------------------------------------------------------
LIB_SRCS    := $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
MAIN_SRC    := $(SRC_DIR)/main.c

# Object mappings
LIB_OBJS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/src/%.o, $(LIB_SRCS))
MAIN_OBJ    := $(BUILD_DIR)/src/main.o

# Test and Tool mappings
TEST_SRCS   := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS   := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/tests/%, $(TEST_SRCS))

TOOL_SRCS   := $(wildcard $(TOOLS_DIR)/*.c)
TOOL_BINS   := $(patsubst $(TOOLS_DIR)/%.c, $(BUILD_DIR)/tools/%, $(TOOL_SRCS))

PERFT_SRCS   := $(wildcard $(PERFT_DIR)/*.c)
PERFT_BINS   := $(patsubst $(PERFT_DIR)/%.c, $(BUILD_DIR)/perft/%, $(PERFT_SRCS))

# ------------------------------------------------------------------------------
# Flags
# ------------------------------------------------------------------------------
COMMON_FLAGS := -std=c23 -I$(INC_DIR) -Wall -Wextra -Wpedantic -Wshadow \
                -Wstrict-aliasing=2 -Wcast-align -Wformat=2 -Wundef \
                -Wunreachable-code -Wmissing-prototypes -Wstrict-prototypes \
                -Wnull-dereference -MMD -MP

DEBUG_FLAGS := $(COMMON_FLAGS) -O1 -g3 -DDEBUG -fsanitize=address,undefined \
               -fno-omit-frame-pointer -fstack-protector-strong
DEBUG_LDFLAGS := -fsanitize=address,undefined

RELEASE_FLAGS := $(COMMON_FLAGS) -O3 -DNDEBUG \
                -mcpu=apple-m1 -flto=full \
                -fomit-frame-pointer -funroll-loops \
                -fstrict-aliasing
RELEASE_LDFLAGS := -flto=full

# Default to Debug for development
CFLAGS  ?= $(DEBUG_FLAGS)
LDFLAGS ?= $(DEBUG_LDFLAGS)

# ------------------------------------------------------------------------------
# Primary Targets
# ------------------------------------------------------------------------------
.DEFAULT_GOAL := all
.PHONY: all build clean run debug test tools help

all build: CFLAGS = $(RELEASE_FLAGS)
all build: LDFLAGS = $(RELEASE_LDFLAGS)
all build: $(BIN)

$(BIN): $(LIB_OBJS) $(MAIN_OBJ)
	@mkdir -p $(@D)
	@echo "[LD]  $@"
	$(CC) $(LDFLAGS) $^ -o $@

# ------------------------------------------------------------------------------
# Pattern Rules (These are key for CLion Visibility)
# ------------------------------------------------------------------------------

# 1. Standard Production Objects
$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 2. Test Objects (Explicitly using TEST_DIR)
$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $<"
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

# 3. Tool Objects
$(BUILD_DIR)/tools/%.o: $(TOOLS_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $<"
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

# 4. Perft Objects (Explicitly using PERFT_DIR)
$(BUILD_DIR)/perft/%.o: $(PERFT_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $<"
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Execution Logic (Tests & Tools)
# ------------------------------------------------------------------------------

# Link a test binary (depends on its object and all library objects)
$(BUILD_DIR)/tests/%: $(BUILD_DIR)/tests/%.o $(LIB_OBJS)
	@echo "[LD]  $@"
	$(CC) $(DEBUG_LDFLAGS) $^ -o $@

# Link a tool binary
$(BUILD_DIR)/tools/%: $(BUILD_DIR)/tools/%.o $(LIB_OBJS)
	@echo "[LD]  $@"
	$(CC) $(DEBUG_LDFLAGS) $^ -o $@

# Link a test binary (depends on its object and all library objects)
$(BUILD_DIR)/perft/%: $(BUILD_DIR)/perft/%.o $(LIB_OBJS)
	@echo "[LD]  $@"
	$(CC) $(DEBUG_LDFLAGS) $^ -o $@

# Run tests
test: $(TEST_BINS)
ifdef FILE
	@echo "[RUN] $(BUILD_DIR)/tests/$(basename $(FILE))"
	@$(BUILD_DIR)/tests/$(basename $(FILE))
else
	@for t in $(TEST_BINS); do echo "[RUN] $$t"; $$t || exit 1; done
endif

# Run tools
tools: $(TOOL_BINS)
ifdef FILE
	@echo "[RUN] $(BUILD_DIR)/tools/$(basename $(FILE))"
	@$(BUILD_DIR)/tools/$(basename $(FILE))
else
	@for t in $(TOOL_BINS); do echo "[RUN] $$t"; $$t; done
endif

# Run perft
perft: $(PERFT_BINS)
ifdef FILE
	@echo "[RUN] $(BUILD_DIR)/perft/$(basename $(FILE))"
	@$(BUILD_DIR)/perft/$(basename $(FILE))
else
	@for t in $(PERFT_BINS); do echo "[RUN] $$t"; $$t || exit 1; done
endif

# ------------------------------------------------------------------------------
# JSON Compilation Database (for CLion/LSP)
# ------------------------------------------------------------------------------
.PHONY: compiledb

compiledb: clean
	@echo "[JSON] Generating compile_commands.json..."
	@bear -- $(MAKE) all test tools perft
	@echo "Done. CLion will now see all headers perfectly."

# ------------------------------------------------------------------------------
# Utilities
# ------------------------------------------------------------------------------
run: CFLAGS = $(RELEASE_FLAGS)
run: LDFLAGS = $(RELEASE_LDFLAGS)
run: build
	@echo "[RUN] $(BIN)"
	$(BIN)

debug: build
	@echo "[DBG] $(BIN)"
	$(BIN)

clean:
	$(RM) $(BUILD_DIR)
	@echo "Cleaned."

help:
	@echo ""
	@echo "  bit-chess Makefile"
	@echo "  ------------------"
	@printf "  %-35s %s\n" "make / make all" "Build debug binary"
	@printf "  %-35s %s\n" "make run" "Build release and run"
	@printf "  %-35s %s\n" "make test" "Run all tests"
	@printf "  %-35s %s\n" "make test FILE=bitboard_test.c" "Run one test"
	@printf "  %-35s %s\n" "make tools" "Run all tools"
	@printf "  %-35s %s\n" "make tools FILE=magic_attacks_generator.c" "Run one tool"
	@printf "  %-35s %s\n" "make perft" "Run all perft"
	@printf "  %-35s %s\n" "make tools FILE=perft_test_pos_1.c" "Run one perft"
	@printf "  %-35s %s\n" "make clean" "Remove build artifacts"
	@echo ""

-include $(wildcard $(BUILD_DIR)/src/*.d)
-include $(wildcard $(BUILD_DIR)/tests/*.d)
-include $(wildcard $(BUILD_DIR)/tools/*.d)
-include $(wildcard $(BUILD_DIR)/perft/*.d)