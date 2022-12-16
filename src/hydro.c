// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "clover.h"
#include "data.h"
#include "definitions.h"
#include "kernels/kernels.h"
#include "report.h"
#include "utils/math.h"
#include "utils/timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void timestep();

void PdV(bool);

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
    // accelerate();
    PdV(false);
    // flux_calc();
    // advection();
    // reset_field();

    advect_x = !advect_x;

    time_val += dt;

    if (summary_frequency != 0 && step % summary_frequency == 0)
      // field_summary();

      if (visit_frequency != 0 && step % visit_frequency == 0)
        // visit();

        // Sometimes there can be a significant start up cost that appears in the first step.
        // Sometimes it is due to the number of MPI tasks, or OpenCL kernel compilation.
        // On the short test runs, this can skew the results, so should be taken into account
        // in recorded run times.
        if (step == 1)
          first_step = timer() - step_time;
        else if (step == 2)
          second_step = timer() - step_time;

    if (time_val + G_SMALL > end_time || step >= end_step) {
      complete = true;
      // field_summary();
      if (visit_frequency != 0)
        // visit();

        wall_clock = timer() - timerstart;
      if (parallel.boss) {
        fprintf(g_out, "\nCalculation completed\n");
        fprintf(g_out, "Clover is finishing\n");
        fprintf(g_out, "Wall clock %f\n", wall_clock);
        fprintf(g_out, "First step overhead %f\n", first_step - second_step);

        printf("Wall clock %f\n", wall_clock);
        printf("First step overhead %f\n", first_step - second_step);
      }

      if (profiler_on) {
        // do profiler stuff
      }

      clover_finalize();
      exit(0);
    }

    if (parallel.boss) {
      wall_clock = timer() - timerstart;
      step_clock = timer() - step_time;

      fprintf(g_out, "Wall clock %f\n", wall_clock);
      printf("Wall clock %f\n", wall_clock);

      cells = grid.x_cells * grid.y_cells;
      rstep = step;
      grind_time = wall_clock / (rstep * cells);
      step_grind = step_clock / cells;

      fprintf(g_out, "Average time per cell %f\n", grind_time);
      fprintf(g_out, "Step time per cell %f\n", step_grind);
      printf("Average time per cell %f\n", grind_time);
      printf("Step time per cell %f\n", step_grind);
    }
  }
}

void timestep() {
  int tile;
  int jldt, kldt;

  double dtlp;
  double x_pos, y_pos, xl_pos, yl_pos;
  double kernel_time;

  char dt_control[8], dtl_control[8];

  int small;
  int fields[NUM_FIELDS];

  dt = G_BIG;
  small = 0;

  if (profiler_on)
    kernel_time = timer();

  for (tile = 0; tile < tiles_per_chunk; tile++) {
    ideal_gas(&chunk.tiles[tile], false);
  }

  if (profiler_on)
    profiler.ideal_gas += timer() - kernel_time;

  memset(fields, 0, NUM_FIELDS * sizeof(int));
  fields[FIELD_PRESSURE] = 1;
  fields[FIELD_ENERGY0] = 1;
  fields[FIELD_DENSITY0] = 1;
  fields[FIELD_XVEL0] = 1;
  fields[FIELD_YVEL0] = 1;
  // update_halo(fields, 1);

  if (profiler_on)
    kernel_time = timer();

  // viscosity();

  if (profiler_on)
    profiler.viscosity += timer() - kernel_time;

  memset(fields, 0, NUM_FIELDS * sizeof(int));
  fields[FIELD_VISCOSITY] = 1;
  // update_halo(fields, 1);

  if (profiler_on)
    kernel_time = timer();

  for (tile = 0; tile < tiles_per_chunk; tile++) {
    //calc_dt(tile, dtlp, dtl_control, xl_pos, yl_pos, jldt, kldt);

    if (dtlp < dt) {
      dt = dtlp;
      strcpy(dt_control, dtl_control);
      x_pos = xl_pos;
      y_pos = yl_pos;
      jdt = jldt;
      kdt = kldt;
    }
  }

  dt = min(dt, min((dtold * dtrise), dtmax));

  // clover_min(dt)
  if (profiler_on)
    profiler.timestep += timer() - kernel_time;

  if (dt < dtmin)
    small = 1;

  if (parallel.boss) {
    const char *format = " Step %7d time %11.7lf control %11s timestep  %9.2e%8d, %8d x %9.2e y %9.2e";
    fprintf(g_out, format, step, time_val, dt_control, dt, jdt, kdt, x_pos, y_pos);
    printf(format, step, time_val, dt_control, dt, jdt, kdt, x_pos, y_pos);
  }

  if (small == 1)
    report_error("timestep", "small timestep");

  dtold = dt;
}

void PdV(bool predict) {}
