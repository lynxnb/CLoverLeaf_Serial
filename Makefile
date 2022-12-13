# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2022 Niccolò Betto

# @brief Makefile for CloverLeaf Serial
# @author Niccolò Betto

# By default gcc will be used. The major compilers have custom flags available.
#
#  export CC=gcc       		# to select the gcc compiler
#  export CC=clang       	# to select the clang compiler
#
# or this works as well:
#
#  make CC=gcc
#  make CC=clang

# usage: make                     # Will make all binaries (clover_leaf, test)
#        make clean               # Will clean up the directory
#        make clover_leaf         # Will make the clover_leaf binary
#        make DEBUG=1             # Will select debug flags
# e.g. make CC=clang DEBUG=1 # will compile with the clang compiler with clang debug flags

SRC = src
BASE_BUILD_DIR = build
BUILD_TYPE = release
ifdef DEBUG
	BUILD_TYPE = debug
endif

BUILD_DIR = $(BASE_BUILD_DIR)/$(BUILD_TYPE)
OBJECT_DIR = $(BUILD_DIR)/obj
BIN_DIR = .

#-----------------------------------------------------
# Source files
#-----------------------------------------------------

# Target specific file exclusions
CLOVER_EXCLUDE = $(SRC)/tests.c
TEST_EXCLUDE = $(SRC)/clover_leaf.c\
               $(SRC)/report.c

SOURCES = $(filter-out $(CLOVER_EXCLUDE), $(wildcard $(SRC)/*.c $(SRC)/*/*.c))
TEST_SOURCES = $(filter-out $(TEST_EXCLUDE), $(wildcard $(SRC)/*.c $(SRC)/*/*.c))

OBJECTS = $(SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.o)

#-----------------------------------------------------
# Compiler
#-----------------------------------------------------

# Use gcc by default
CC ?= gcc

# Marker for the last compiler used
CC_MARKER = $(BUILD_DIR)/$(CC).built

#-----------------------------------------------------
# Compiler flags
#-----------------------------------------------------

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
  I3E=$(I3E_$(CC))
endif

CFLAGS = $(CFLAGS_$(CC)) $(I3E)

#-----------------------------------------------------
# Targets
#-----------------------------------------------------

.PHONY: all clean run run-test

all: clover_leaf test

clover_leaf: $(BUILD_DIR) $(CC_MARKER) $(OBJECT_DIR) Makefile $(OBJECTS)
	@echo Linking $@ executable...
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@
	@echo Done.

test: $(BUILD_DIR) $(CC_MARKER) $(OBJECT_DIR) Makefile $(TEST_OBJECTS)
	@echo Linking $@ executable...
	@$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(BIN_DIR)/$@
	@echo Done building tests.

$(OBJECT_DIR)/%.o: $(SRC)/%.c Makefile $(CC_MARKER)
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(CC_MARKER): Makefile
	@rm -f $(BUILD_DIR)/*.built
	@touch $(CC_MARKER)

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
