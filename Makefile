# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2022 Niccolò Betto

# @brief Makefile for CloverLeaf Cereal
# @author Niccolò Betto

# By default gcc will be used. The major compilers have custom flags available.
#
#  export COMPILER=gcc       		# to select the gcc flags
#  export COMPILER=clang       	# to select the clang flags
#
# or this works as well:
#
#  make COMPILER=gcc
#  make COMPILER=clang

# usage: make                     # Will make all binaries (clover_leaf, test)
#        make clean               # Will clean up the directory
#        make clover_leaf         # Will make the clover_leaf binary
#        make DEBUG=1             # Will select debug options. If a compiler is selected, it will use compiler specific debug options
# e.g. make COMPILER=clang DEBUG=1 # will compile with the clang compiler with clang debug flags

BASE_BUILD_DIR = build
BUILD_TYPE = release
ifdef DEBUG
	BUILD_TYPE = debug
endif

BUILD_DIR = $(BASE_BUILD_DIR)/$(BUILD_TYPE)
OBJECT_DIR = $(BUILD_DIR)/obj
BIN_DIR = .

# Use gcc by default
COMPILER ?= gcc

# Marker for the last compiler used
COMPILER_MARKER = $(BUILD_DIR)/$(COMPILER).built

# C compile flags
CFLAGS_gcc   = -O3 -march=native -funroll-loops
CFLAGS_clang = -O3 -march=native -funroll-loops
CFLAGS_taffo = -O3 -march=native -funroll-loops

ifdef DEBUG
  CFLAGS_gcc   = -Og -g -Wall -Wextra -fbounds-check
  CFLAGS_clang = -Og -g -Wall -Wextra -fbounds-check
  CFLAGS_taffo = -Og -g -Wall -Wextra -fbounds-check
endif

ifdef IEEE
  I3E_gcc       = -ffloat-store
  I3E_clang     = -ffloat-store
  I3E_taffo     = -ffloat-store
  I3E=$(I3E_$(COMPILER))
endif

CFLAGS = $(CFLAGS_$(COMPILER)) $(I3E)

SOURCES = $(filter-out tests.c, $(wildcard *.c))
TEST_SOURCES = $(filter-out clover_leaf.c report.c, $(wildcard *.c))

OBJECTS = $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCES:.c=.o)))
TEST_OBJECTS = $(addprefix $(OBJECT_DIR)/, $(notdir $(TEST_SOURCES:.c=.o)))

.PHONY: all clean run run-test

all: clover_leaf test

clover_leaf: $(BUILD_DIR) $(COMPILER_MARKER) $(OBJECT_DIR) Makefile $(OBJECTS)
	@echo Linking final executable...
	@$(COMPILER) $(CFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@
	@echo Done.

test: $(BUILD_DIR) $(COMPILER_MARKER) $(OBJECT_DIR) Makefile $(TEST_OBJECTS)
	@echo Linking final executable...
	@$(COMPILER) $(CFLAGS) $(TEST_OBJECTS) -o $(BIN_DIR)/$@
	@echo Done building tests.

$(OBJECT_DIR)/%.o: %.c Makefile $(COMPILER_MARKER)
	@echo Compiling $<
	@$(COMPILER) $(CFLAGS) -c $< -o $@

$(COMPILER_MARKER): Makefile
	@rm -f $(BUILD_DIR)/*.built
	@touch $(COMPILER_MARKER)

$(OBJECT_DIR):
	@mkdir -p $(OBJECT_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

run: clover_leaf
	@./clover_leaf

run-test: test
	@./test

clean:
	@rm -rf $(BASE_BUILD_DIR)
	@rm -f clover_leaf
	@rm -f test
