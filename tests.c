// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include "definitions.h"
#include "parse.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "read_input.h"
#include "tests.h"

void test_parse_getword() {
  char test[16] = " test_problem 2\0";

  parse_init(file_in, "");
  line = test;
  printf("Parsing: '%s'\n", line);

  fflush(stdout);

  char *word = parse_getword(false);
  printf("Result: '%s'\n", word);
}

/**
 * @brief Print the whole clover.in file using parse_getline
 */
void test_parse_getline() {
  parse_init(file_in, "*clover");

  while (parse_getline(0) == 0) {
    printf("Parsing: '%s'\n", line);
  }
}

void test_parse_getline_getword() {
  parse_init(file_in, "*clover");

  while (parse_getline(0) == 0) {
    char l[100];
    strcpy(l, line);
    char *word = parse_getword(false);
    printf("Word: '%s' in line '%s'\n", word, l);
  }
  parse_getword(true);
}

void test_parse_file() {
  parallel.boss = true;
  parallel.max_task = 1;

  printf("Reading clover.in\n");
  g_in = file_in;
  g_out = stdout;
  read_input();
}

int main(int argc, char **argv) {
  puts("*** CloverLeaf unit test runner (C version) ***");
  file_in = fopen("clover.in", "r");

  RUN_TEST(test_parse_getword);
  RUN_TEST(test_parse_getline);
  RUN_TEST(test_parse_getline_getword);
  RUN_TEST(test_parse_file);

  return 0;
}
