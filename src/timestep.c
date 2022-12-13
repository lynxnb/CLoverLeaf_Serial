// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include "definitions.h"
#include "kernels/kernels.h"
#include "report.h"
#include "utils/math.h"
#include "utils/timer.h"
#include <stdio.h>
#include <string.h>

void update_halo(int fields[static NUM_FIELDS], int depth);

void viscosity();

void calc_dt(int tile, double local_dt, char local_control[static 8], double xl_pos, double yl_pos, int jldt, int kldt);

void timestep() {
  int tile;
  int jldt, kldt;

  double dtlp;
  double x_pos, y_pos, xl_pos, yl_pos;
  double kernel_time;

  char dt_control[8], dtl_control[8];

  int small;
  int fields[NUM_FIELDS];

  dt = g_big;
  small = 0;

  if (profiler_on)
    kernel_time = timer();

  for (tile = 0; tile < tiles_per_chunk; tile++) {
    ideal_gas(tile, false);
  }

  if (profiler_on)
    profiler.ideal_gas += timer() - kernel_time;

  memset(fields, 0, NUM_FIELDS * sizeof(int));
  fields[FIELD_PRESSURE] = 1;
  fields[FIELD_ENERGY0] = 1;
  fields[FIELD_DENSITY0] = 1;
  fields[FIELD_XVEL0] = 1;
  fields[FIELD_YVEL0] = 1;
  update_halo(fields, 1);

  if (profiler_on)
    kernel_time = timer();

  viscosity();

  if (profiler_on)
    profiler.viscosity += timer() - kernel_time;

  memset(fields, 0, NUM_FIELDS * sizeof(int));
  fields[FIELD_VISCOSITY] = 1;
  update_halo(fields, 1);

  if (profiler_on)
    kernel_time = timer();

  for (tile = 0; tile < tiles_per_chunk; tile++) {
    calc_dt(tile, dtlp, dtl_control, xl_pos, yl_pos, jldt, kldt);

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
    fprintf(g_out, format, step, time, dt_control, dt, jdt, kdt, x_pos, y_pos);
    printf(format, step, time, dt_control, dt, jdt, kdt, x_pos, y_pos);
  }

  if (small == 1)
    report_error("timestep", "small timestep");

  dtold = dt;
}

void update_halo(int fields[static NUM_FIELDS], int depth) {
  // update_halo_kernel
}

void viscosity() {
  // viscosity_kernel
}

void calc_dt(int tile, double local_dt, char local_control[static 8], double xl_pos, double yl_pos, int jldt,
             int kldt) {
  int small = 0, l_control;
  local_dt = g_big;

  // calc_dt_kernel

  switch (l_control) {
  case 1:
    strcpy(local_control, "sound");
    break;
  case 2:
    strcpy(local_control, "xvel");
    break;
  case 3:
    strcpy(local_control, "yvel");
    break;
  case 4:
    strcpy(local_control, "div");
    break;
  }
}
