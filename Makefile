#Crown Copyright 2012 AWE.
#
# This file is part of CloverLeaf.
#
# CloverLeaf is free software: you can redistribute it and/or modify it under 
# the terms of the GNU General Public License as published by the 
# Free Software Foundation, either version 3 of the License, or (at your option) 
# any later version.
#
# CloverLeaf is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
# details.
#
# You should have received a copy of the GNU General Public License along with 
# CloverLeaf. If not, see http://www.gnu.org/licenses/.
#
# @brief Makefile for CloverLeaf
# @author Wayne Gaudin, Andy Herdman, Niccolò Betto
# @details Agnostic, platform independent makefile for the Clover Leaf benchmark code.
#
# It is not meant to be smart as to only recompile what's necessary.
#
# There is no single way of turning OpenMP compilation on with all compilers.
# The major compilers have been added as a variable. By default make will use gcc.
# To select a OpenMP compiler option, do this in the shell before typing make:-
#
#  export COMPILER=gcc       		# to select the gcc flags
#  export COMPILER=clang       	# to select the clang flags
#  export COMPILER=taffo       	# to select the taffo flags
#
# or this works as well:-
#
#  make COMPILER=gcc
#  make COMPILER=clang
#  make COMPILER=taffo
#
# usage: make                     # Will make all binaries (clover_leaf, test)
#        make clean               # Will clean up the directory
#        make DEBUG=1             # Will select debug options. If a compiler is selected, it will use compiler specific debug options
# e.g. make COMPILER=gcc DEBUG=1 # will compile with the gcc compiler with gcc debug flags

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
CFLAGS_      = -O3

ifdef DEBUG
  CFLAGS_gcc   = -Og -g -Wall -Wextra -fbounds-check
  CFLAGS_clang = -Og -g -Wall -Wextra -fbounds-check
  CFLAGS_taffo = -Og -g -Wall -Wextra -fbounds-check
  CFLAGS_      = -Og -g
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
