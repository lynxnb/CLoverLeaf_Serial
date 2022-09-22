// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include "definitions.h"
#include "math.h"
#include "parse.h"
#include "report.h"
#include "string_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_input() {
  int state, stat, state_max;
  double dx, dy;
  char *word;

  test_problem = 0;
  state_max = 0;

  grid.xmin = 0.0;
  grid.ymin = 0.0;
  grid.ymax = 100.0;
  grid.xmax = 100.0;

  grid.x_cells = 10;
  grid.y_cells = 10;

  end_time = 10.0;
  end_step = g_ibig;
  complete = false;

  visit_frequency = 0;
  summary_frequency = 0;

  tiles_per_chunk = 1;

  dtinit = 0.1;
  dtmax = 1.0;
  dtmin = 0.0000001;
  dtrise = 1.5;
  dtc_safe = 0.7;
  dtu_safe = 0.5;
  dtv_safe = 0.5;
  dtdiv_safe = 0.7;

  use_fortran_kernels = false;
  use_C_kernels = true;
  use_OA_kernels = false;
  profiler_on = false;
  profiler.timestep = 0.0;
  profiler.acceleration = 0.0;
  profiler.PdV = 0.0;
  profiler.cell_advection = 0.0;
  profiler.mom_advection = 0.0;
  profiler.viscosity = 0.0;
  profiler.ideal_gas = 0.0;
  profiler.visit = 0.0;
  profiler.summary = 0.0;
  profiler.reset = 0.0;
  profiler.revert = 0.0;
  profiler.flux = 0.0;
  profiler.tile_halo_exchange = 0.0;
  profiler.self_halo_exchange = 0.0;
  profiler.mpi_halo_exchange = 0.0;

  if (parallel.boss)
    fputs("Reading input file\n\n", g_out);

  stat = parse_init(g_in, "*clover");

  while (true) {
    stat = parse_getline(0);
    if (stat != 0)
      break;
    while (true) {
      word = parse_getword(false);
      if (!strcmp(word, ""))
        break;

      if (!strcmp(word, "state")) {
        state_max = max(state_max, parse_getival(parse_getword(true)));
        break;
      }
    }
  }

  number_of_states = state_max;

  if (number_of_states < 1)
    report_error("read_input", "No states defined.");

  stat = parse_init(g_in, "*clover");

  states = (state_type *)malloc(number_of_states * sizeof(state_type));
  for (int i = 0; i < number_of_states; i++) {
    states[i].defined = false;
    states[i].energy = 0.0;
    states[i].density = 0.0;
    states[i].xvel = 0.0;
    states[i].yvel = 0.0;
  }

  while (true) {
    stat = parse_getline(0);
    if (stat != 0)
      break;

    while (true) {
      word = parse_getword(false);
      if (!strcmp(word, ""))
        break;

      // clang-format off
      sswitch(word) {
        scase("initial_timestep")
          dtinit = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "initial timestep %lf\n", dtinit);
          break;
        scase("max_timestep")
          dtmax = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "max timestep %lf\n", dtmax);
          break;
        scase("timestep_rise")
          dtrise = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "timestep_rise %lf\n", dtrise);
          break;
        scase("end_time")
          end_time = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "end_time %lf\n", end_time);
          break;
        scase("end_step")
          end_step = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "end_step %d\n", end_step);
          break;
        scase("xmin")
          grid.xmin = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "xmin %lf\n", grid.xmin);
          break;
        scase("xmax")
          grid.xmax = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "xmax %lf\n", grid.xmax);
          break;
        scase("ymin")
          grid.ymin = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "ymin %lf\n", grid.ymin);
          break;
        scase("ymax")
          grid.ymax = parse_getrval(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "ymax %lf\n", grid.ymax);
          break;
        scase("x_cells")
          grid.x_cells = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "x_cells %d\n", grid.x_cells);
          break;
        scase("y_cells")
          grid.y_cells = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "y_cells %d\n", grid.y_cells);
          break;
        scase("visit_frequency")
          visit_frequency = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "visit_frequency %d\n", visit_frequency);
          break;
        scase("summary_frequency")
          summary_frequency = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "summary_frequency %d\n", summary_frequency);
          break;
        scase("tiles_per_chunk")
          tiles_per_chunk = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "tiles_per_chunk %d\n", tiles_per_chunk);
          break;
        scase("tiles_per_problem")
          tiles_per_chunk = parse_getival(parse_getword(true)) / parallel.max_task;
          if (parallel.boss)
            fprintf(g_out, "tiles_per_chunk %d\n", tiles_per_chunk);
          break;
        scase("use_fortran_kernels")
          use_fortran_kernels = true;
          use_C_kernels = false;
          use_OA_kernels = false;
          break;
        scase("use_c_kernels")
          use_fortran_kernels = false;
          use_C_kernels = true;
          use_OA_kernels = false;
          break;
        scase("use_oa_kernels")
          use_fortran_kernels = false;
          use_C_kernels = false;
          use_OA_kernels = true;
          break;
        scase("profiler_on")
          profiler_on = true;
          if (parallel.boss)
            fputs("Profiler_on\n", g_out);
          break;
        scase("test_problem")
          test_problem = parse_getival(parse_getword(true));
          if (parallel.boss)
            fprintf(g_out, "test_problem %d\n", test_problem);
          break;
        scase("state")
          state = parse_getival(parse_getword(true));

          if (parallel.boss) {
            fprintf(g_out, "Reading specification for state %d\n\n", state);
            if (states[state].defined)
              report_error("read_input", "State defined twice.");  
          }

          states[state].defined = true;
          while (true) {
            word = parse_getword(false);
            if (!strcmp(word, ""))
              break;

            sswitch(word) {
              scase("xvel")
                states[state].xvel = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "xvel %lf\n", states[state].xvel);
                break;
              scase("yvel")
                states[state].yvel = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "yvel %lf\n", states[state].yvel);
                break;
              scase("xmin")
                states[state].xmin = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state xmin %lf\n", states[state].xmin);
                break;
              scase("ymin")
                states[state].ymin = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state ymin %lf\n", states[state].ymin);
                break;
              scase("xmax")
                states[state].xmax = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state xmax %lf\n", states[state].xmax);
                break;
              scase("ymax")
                states[state].ymax = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state ymax %lf\n", states[state].ymax);
                break;
              scase("radius")
                states[state].radius = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state radius %lf\n", states[state].radius);
                break;
              scase("density")
                states[state].density = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state density %lf\n", states[state].density);
                break;
              scase("energy")
                states[state].energy = parse_getrval(parse_getword(true));
                if (parallel.boss)
                  fprintf(g_out, "state energy %lf\n", states[state].energy);
                break;
              scase("geometry")
                word = trim(parse_getword(true));
                sswitch(word) {
                  scase("rectangle")
                    states[state].geometry = g_rect;
                    if (parallel.boss)
                      fputs("state geometry rectangular\n", g_out);
                    break;
                  scase("circle")
                    states[state].geometry = g_circ;
                    if (parallel.boss)
                      fputs("state geometry circular\n", g_out);
                    break;
                  scase("point")
                    states[state].geometry = g_point;
                    if (parallel.boss)
                      fputs("state geometry point\n", g_out);
                    break;
                } sswitch_end;
            } sswitch_end; // state switch
          } // case("state") while loop

          if (parallel.boss)
            fputc('\n', g_out);
          break;
      } sswitch_end;
      // clang-format on
    }
  }

  if (parallel.boss) {
    fputc('\n', g_out);
    if (use_fortran_kernels) {
      fputs("Fortran kernels were requested but they are not available\n", g_out);
      use_fortran_kernels = false;
      use_C_kernels = true;
      fputs("Using C Kernels\n", g_out);
    } else if (use_C_kernels) {
      fputs("Using C Kernels\n", g_out);
    } else if (use_OA_kernels) {
      fputs("Using OpenACC Kernels\n", g_out);
    }

    fputs("\nInput read finished.\n", g_out);
  }

  dx = (grid.xmax - grid.xmin) / (float)grid.x_cells;
  dy = (grid.ymax - grid.ymin) / (float)grid.y_cells;
  for (int i = 2; i < number_of_states; i++) {
    states[i].xmin = states[i].xmin + (dx / 100.0);
    states[i].ymin = states[i].ymin + (dy / 100.0);
    states[i].xmax = states[i].xmax - (dx / 100.0);
    states[i].ymax = states[i].ymax - (dy / 100.0);
  }
}
