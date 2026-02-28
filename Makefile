# ==============================================================================
# Makefile for bit-chess — Bitboard Chess Engine
# Compiler: Apple Clang 17 / arm64-apple-darwin
# ==============================================================================

# ------------------------------------------------------------------------------
# Project layout
# ------------------------------------------------------------------------------
TARGET      := bit-chess
SRC_DIR     := src
INC_DIR     := include
TEST_DIR    := tests
TOOLS_DIR   := tools
BUILD_DIR   := build

# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
CC      := clang
RM      := rm -rf

# ------------------------------------------------------------------------------
# Source / object discovery
# ------------------------------------------------------------------------------
# All production sources, excluding main.c
LIB_SRCS    := $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
MAIN_SRC    := $(SRC_DIR)/main.c

LIB_OBJS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/src/%.o, $(LIB_SRCS))
MAIN_OBJ    := $(BUILD_DIR)/src/main.o

TEST_SRCS   := $(wildcard $(TEST_DIR)/*.c)
TOOL_SRCS   := $(wildcard $(TOOLS_DIR)/*.c)

# ------------------------------------------------------------------------------
# Flags — common base
# ------------------------------------------------------------------------------
COMMON_FLAGS := \
    -std=c23                    \
    -I$(INC_DIR)                \
    -Wall                       \
    -Wextra                     \
    -Wpedantic                  \
    -Wshadow                    \
    -Wstrict-aliasing=2         \
    -Wcast-align                \
    -Wformat=2                  \
    -Wundef                     \
    -Wunreachable-code          \
    -Wmissing-prototypes        \
    -Wstrict-prototypes         \
    -Wnull-dereference          \
    -MMD -MP                    \

# Debug build — fast compilation, sanitisers, full warnings
DEBUG_FLAGS := \
    $(COMMON_FLAGS)             \
    -O1                         \
    -g3                         \
    -DDEBUG                     \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer     \
    -fstack-protector-strong    \

DEBUG_LDFLAGS := -fsanitize=address,undefined

# Release / run build — max optimisation, no assertions
RELEASE_FLAGS := \
    $(COMMON_FLAGS)             \
    -O3                         \
    -DNDEBUG                    \
    -march=native               \
    -flto                       \
    -fomit-frame-pointer        \

RELEASE_LDFLAGS := -flto

# Test / tools build — same as debug but no sanitisers to keep linking simple
#   (change to DEBUG_FLAGS if you want ASAN in tests too)
TEST_FLAGS  := $(COMMON_FLAGS) -O1 -g3 -DDEBUG -fstack-protector-strong

# Default build flags (used by `all` / `build`)
CFLAGS      ?= $(DEBUG_FLAGS)
LDFLAGS     ?= $(DEBUG_LDFLAGS)

# ------------------------------------------------------------------------------
# Dependency files
# ------------------------------------------------------------------------------
DEPS := \
    $(wildcard $(BUILD_DIR)/src/*.d)       \
    $(wildcard $(BUILD_DIR)/tests/*.d)     \
    $(wildcard $(BUILD_DIR)/tools/*.d)     \

# ==============================================================================
# Default goal
# ==============================================================================
.DEFAULT_GOAL := all

.PHONY: all build clean run debug test tools help

# ==============================================================================
# 1 & 2. all / build  — debug-flagged binary (good default for development)
# ==============================================================================
all build: CFLAGS  = $(DEBUG_FLAGS)
all build: LDFLAGS = $(DEBUG_LDFLAGS)
all build: $(TARGET)

$(TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	@echo "[LD]  $@"
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "Done → ./$@"

# ------------------------------------------------------------------------------
# Compile production sources → build/src/
# ------------------------------------------------------------------------------
$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $<"
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
# 3. clean
# ==============================================================================
clean:
	$(RM) $(BUILD_DIR) $(TARGET)
	@echo "Cleaned."

# ==============================================================================
# 7. run  — release binary, max optimisation, assertions disabled
# ==============================================================================
run: CFLAGS  = $(RELEASE_FLAGS)
run: LDFLAGS = $(RELEASE_LDFLAGS)
run: $(TARGET)
	@echo "[RUN] ./$(TARGET)"
	./$(TARGET)

# ==============================================================================
# 8. debug  — debug binary with sanitisers; runs immediately
# ==============================================================================
debug: CFLAGS  = $(DEBUG_FLAGS)
debug: LDFLAGS = $(DEBUG_LDFLAGS)
debug: $(TARGET)
	@echo "[DBG] ./$(TARGET)"
	./$(TARGET)

# ==============================================================================
# 4. test  — compile every tests/*_test.c against lib objects and run each
# ==============================================================================

# Shared lib objects used by tests (built into build/tests/src/)
TEST_LIB_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/tests/src/%.o, $(LIB_SRCS))

$(BUILD_DIR)/tests/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $< (test)"
	$(CC) $(TEST_FLAGS) -c $< -o $@

# Pattern to build a single test executable
# Usage: $(call build_test, tests/foo_test.c) → build/tests/foo_test
define build_and_run_test
	$(eval _src  := $(1))
	$(eval _name := $(basename $(notdir $(_src))))
	$(eval _obj  := $(BUILD_DIR)/tests/$(_name).o)
	$(eval _bin  := $(BUILD_DIR)/tests/$(_name))
	@mkdir -p $(BUILD_DIR)/tests
	@echo "[CC]  $(_src)"
	$(CC) $(TEST_FLAGS) -c $(_src) -o $(_obj)
	@echo "[LD]  $(_bin)"
	$(CC) $(TEST_FLAGS) $(_obj) $(TEST_LIB_OBJS) -o $(_bin)
	@echo "[RUN] $(_bin)"
	$(_bin)
	@echo ""
endef

test: $(TEST_LIB_OBJS)
ifdef FILE
	$(call build_and_run_test,$(TEST_DIR)/$(FILE))
else
	@$(foreach src,$(TEST_SRCS),$(call build_and_run_test,$(src)))
endif

# ==============================================================================
# 5. test FILE=<name>  — run a single test file
#    Usage:  make test FILE=move_gen_test.c
# ==============================================================================
# Handled by the ifdef FILE branch above.

# ==============================================================================
# 6. tools  — compile & run every tools/*.c sequentially
# ==============================================================================
TOOLS_LIB_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/tools/src/%.o, $(LIB_SRCS))

$(BUILD_DIR)/tools/src/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC]  $< (tools)"
	$(CC) $(RELEASE_FLAGS) -c $< -o $@

define build_and_run_tool
	$(eval _src  := $(1))
	$(eval _name := $(basename $(notdir $(_src))))
	$(eval _obj  := $(BUILD_DIR)/tools/$(_name).o)
	$(eval _bin  := $(BUILD_DIR)/tools/$(_name))
	@mkdir -p $(BUILD_DIR)/tools
	@echo "[CC]  $(_src)"
	$(CC) $(RELEASE_FLAGS) -c $(_src) -o $(_obj)
	@echo "[LD]  $(_bin)"
	$(CC) $(RELEASE_FLAGS) $(_obj) $(TOOLS_LIB_OBJS) -o $(_bin)
	@echo "[RUN] $(_bin)"
	$(_bin)
	@echo ""
endef

tools: $(TOOLS_LIB_OBJS)
ifdef FILE
	$(call build_and_run_tool,$(TOOLS_DIR)/$(FILE))
else
	@$(foreach src,$(TOOL_SRCS),$(call build_and_run_tool,$(src)))
endif

# ==============================================================================
# Help
# ==============================================================================
help:
	@echo ""
	@echo "  bit-chess Makefile"
	@echo "  ------------------"
	@echo "  make / make all          		Build debug binary → ./$(TARGET)"
	@echo "  make build               		Same as all"
	@echo "  make run                 		Build optimised release binary and run"
	@echo "  make debug               		Build debug binary and run"
	@echo "  make test                		Build and run ALL tests in tests/"
	@echo "  make test FILE=foo_test.c 		Build and run a single test"
	@echo "  make tools               		Build and run ALL tools in tools/"
	@echo "  make tools FILE=magic_generator.c	Build and run a single tool"
	@echo "  make clean               		Remove build/ and $(TARGET)"
	@echo ""

# ==============================================================================
# Auto-generated dependency inclusion
# ==============================================================================
-include $(DEPS)