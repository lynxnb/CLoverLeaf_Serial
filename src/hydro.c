// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include "definitions.h"
#include "utils/timer.h"

extern void timestep();

void hydro() {
  int loc = 1;
  double timerstart, wall_clock, step_clock;
  double grind_time, cells, rstep;
  double step_time, step_grind;
  double first_step, second_step;
  double kerner_total;
  double totals[parallel.max_task];

  timerstart = timer();

  while (true) {
    step_time = timer();
    step++;

    timestep();
    PdV(true);
  }
}
