// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include <stdio.h>
#include <stdlib.h>

void clover_init_comms() {
  const int rank = 0, size = 1;

  parallel.parallel = true;
  parallel.task = rank;

  if (rank == 0)
    parallel.boss = true;

  parallel.boss_task = 0;
  parallel.max_task = size;
}

void clover_abort() {
  if (g_in != NULL)
    fclose(g_in);

  if (g_out != NULL)
    fclose(g_out);

  exit(1);
}

void clover_min(double dt) {
  // empty
}
