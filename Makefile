# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2022 Niccolò Betto

# @brief Makefile for CloverLeaf Serial
# @author Niccolò Betto

# By default gcc will be used. The major compilers have custom flags available.
#
#  export CC=gcc          # to select the gcc compiler
#  export CC=clang        # to select the clang compiler
#
# or this works as well:
#
#  make CC=gcc
#  make CC=clang

# usage: make                     # Will make all binaries (clover_leaf, test)
#        make clean               # Will clean up the directory
#        make clover_leaf         # Will make the clover_leaf binary
#        make test                # Will make the test binary
#        make run                 # Will make and run the clover_leaf binary
#        make run-test            # Will make and run the test binary
#        make DEBUG=1             # Will select debug flags
#        make USER_CALLBACKS=1    # Will compile with user callbacks enabled (see user_callbacks.h)
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
TAFFO_DIR = $(BASE_BUILD_DIR)/taffo

#-----------------------------------------------------
# Source files
#-----------------------------------------------------

# Target-specific file exclusions
CLOVER_EXCLUDE = $(SRC)/tests.c\
                 $(SRC)/tests.h
TEST_EXCLUDE = $(SRC)/clover_leaf.c\
               $(SRC)/report.c

SOURCES = $(filter-out $(CLOVER_EXCLUDE), $(wildcard $(SRC)/*.c $(SRC)/*/*.c))
HEADERS = $(filter-out $(CLOVER_EXCLUDE), $(wildcard $(SRC)/*.h $(SRC)/*/*.h))
TEST_SOURCES = $(filter-out $(TEST_EXCLUDE), $(wildcard $(SRC)/*.c $(SRC)/*/*.c))

OBJECTS = $(SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.o)
DEPENDS = $(SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.d)
TEST_OBJECTS = $(TEST_SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.o)
TEST_DEPENDS = $(TEST_SOURCES:$(SRC)/%.c=$(OBJECT_DIR)/%.d)

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

CFLAGS = -O3 -march=native -funroll-loops -Wno-attributes
TAFFO_FLAGS = -Wno-unused-command-line-argument -temp-dir $(TAFFO_DIR)

ifdef DEBUG
	CFLAGS = -Og -g -fbounds-check
endif

ifdef IEEE
	I3E = -ffloat-store
endif

# Lib / Includes
LIBS = -lm

CFLAGS += $(I3E)

ifdef USER_CALLBACKS
	CFLAGS += -DUSER_CALLBACKS_ENABLED
endif

#-----------------------------------------------------
# Targets
#-----------------------------------------------------

.PHONY: all clean run run-test run-taffo

all: clover_leaf clover_leaf_taffo test

clover_leaf: $(BUILD_DIR) $(CC_MARKER) $(OBJECT_DIR) Makefile $(OBJECTS)
	@echo Linking $@ executable...
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@ $(LIBS)
	@echo Done.

clover_leaf_taffo: $(TAFFO_DIR) $(SOURCES) $(HEADERS) Makefile
	@echo Compiling all source files with TAFFO...
	@taffo $(CFLAGS) $(TAFFO_FLAGS) $(SOURCES) -o $(BIN_DIR)/$@ $(LIBS)
	@taffo $(CFLAGS) $(TAFFO_FLAGS) $(SOURCES) -o $(BIN_DIR)/clover_leaf_taffo $(LIBS)
	@echo Done.

test: $(BUILD_DIR) $(CC_MARKER) $(OBJECT_DIR) Makefile $(TEST_OBJECTS)
	@echo Linking $@ executable...
	@$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(BIN_DIR)/$@ $(LIBS)
	@echo Done building tests.

-include $(DEPENDS)
-include $(TEST_DEPENDS)

$(OBJECT_DIR)/%.o: $(SRC)/%.c Makefile $(CC_MARKER)
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(CC_MARKER): Makefile
	@rm -f $(BUILD_DIR)/*.built
	@touch $(CC_MARKER)

$(OBJECT_DIR):
	@mkdir -p $(OBJECT_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TAFFO_DIR):
	@mkdir -p $(TAFFO_DIR)	

run: clover_leaf
	@./clover_leaf

run-taffo: clover_leaf_taffo
	@./clover_leaf_taffo

run-test: test
	@./test

clean:
	@rm -rf $(BASE_BUILD_DIR)
	@rm -f clover_leaf*
	@rm -f clover_leaf_taffo*
	@rm -f test*
