// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "clover.h"
#include "data.h"
#include "definitions.h"
#include "kernels/kernels.h"
#include "parse.h"
#include "report.h"
#include "types/definitions.h"
#include "utils/math.h"
#include "utils/string.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_input();

void start();

/**
 * @file allocate.c
 */
extern void build_field();

/**
 * @brief Top level initialisation routine
 * @details Checks for the user input and either invokes the input reader or switches to the internal test problem. It
 * processes the input and strips comments before writing a final input file. It then calls the start routine.
 */
void initialise() {
  FILE *uin = NULL, *out_unit = NULL;

  if (parallel.boss) {
    errno = 0;
    g_out = fopen("clover.out", "w");
    if (errno != 0) {
      g_out = NULL;
      report_error("initialise", "Error opening clover.out file.");
    }

    fprintf(g_out, "Clover Version %f\nMPI Version\nTask Count %d\n", G_VERSION, parallel.max_task);

    puts("Output file clover.out opened. All output will go there.");

    fputs("\nClover will run from the following input:-\n", g_out);

    errno = 0;
    uin = fopen("clover.in", "r");
    if (errno != 0) {
      errno = 0;
      out_unit = fopen("clover.in", "w");
      if (errno != 0)
        report_error("initialise", "Error opening clover.in file");

      fputs("*clover\n"
            " state 1 density=0.2 energy=1.0\n"
            " state 2 density=1.0 energy=2.5 geometry=rectangle xmin=0.0"
            " xmax=5.0 ymin=0.0 ymax=2.0\n"
            " x_cells=10\n"
            " y_cells=2\n"
            " xmin=0.0\n"
            " ymin=0.0\n"
            " xmax=10.0\n"
            " ymax=2.0\n"
            " initial_timestep=0.04\n"
            " timestep_rise=1.5\n"
            " max_timestep=0.04\n"
            " end_time=3.0\n"
            " test_problem 1\n"
            "*endclover\n",
            out_unit);

      fclose(out_unit);
      uin = fopen("clover.in", "r");
    }

    errno = 0;
    out_unit = fopen("clover.in.tmp", "w");
    if (errno != 0)
      report_error("initialise", "Error opening clover.in.tmp file");

    int stat = parse_init(uin, "");
    while (true) {
      stat = parse_getline(-1);
      if (stat != 0)
        break;
      fputs(line, out_unit);
      fputc('\n', out_unit);
    }
    fclose(out_unit);
  }

  errno = 0;
  g_in = fopen("clover.in.tmp", "r");
  if (errno != 0)
    report_error("initialise", "Error opening clover.in.tmp file");

  if (parallel.boss) {
    // Write input file to output file
    rewind(uin);
    char buf[100];
    while (true) {
      if (fgets(buf, 100, uin) == NULL)
        break;
      fputs(buf, g_out);
    }

    fputs("\nInitialising and generating\n\n", g_out);
  }

  read_input();

  step = 0;

  start();

  if (parallel.boss)
    fputs("Starting the calculation", g_out);

  fclose(g_in);
}

/**
 * @brief Reads the user input
 * @details Reads and parses the user input from the processed file and sets the variables used in the generation phase.
 * Default values are also set here.
 */
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
  end_step = G_IBIG;
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

  states = malloc(number_of_states * sizeof(state_type));
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
          // Subtract 1 because the state number in the input file is 1-based,
          // but the state number in the code is 0-based
          state = parse_getival(parse_getword(true)) - 1;

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
                    states[state].geometry = G_RECT;
                    if (parallel.boss)
                      fputs("state geometry rectangular\n", g_out);
                    break;
                  scase("circle")
                    states[state].geometry = G_CIRC;
                    if (parallel.boss)
                      fputs("state geometry circular\n", g_out);
                    break;
                  scase("point")
                    states[state].geometry = G_POINT;
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

  // If a state boundary falls exactly on a cell boundary then round off can
  // cause the state to be put one cell further that expected. This is compiler-
  // system dependent. To avoid this, a state boundary is reduced/increased by a 100th
  // of a cell width so it lies well with in the intended cell.
  // Because a cell is either full or empty of a specified state, this small
  // modification to the state extents does not change the answers.
  dx = (grid.xmax - grid.xmin) / (float)grid.x_cells;
  dy = (grid.ymax - grid.ymin) / (float)grid.y_cells;
  for (int i = 2; i < number_of_states; i++) {
    states[i].xmin = states[i].xmin + (dx / 100.0);
    states[i].ymin = states[i].ymin + (dy / 100.0);
    states[i].xmax = states[i].xmax - (dx / 100.0);
    states[i].ymax = states[i].ymax - (dy / 100.0);
  }
}

/**
 * @brief Main set up routine
 * @details Invokes the mesh decomposer and sets up chunk connectivity. It then allocates the communication buffers and
 * call the chunk initialisation and generation routines. It calls the equation of state to calculate initial pressure
 * before priming the halo cells and writing an initial field summary.
 */
void start() {
  int c, tile;

  int x_cells, y_cells;
  int right, left, top, bottom;

  int fields[NUM_FIELDS];

  bool profiler_off;

  if (parallel.boss) {
    fputs("Setting up initial geometry\n", g_out);
  }

  time_val = 0.0;
  step = 0;
  dtold = dtinit;
  dt = dtinit;

  number_of_chunks = clover_get_num_chunks();
  clover_decompose(grid.x_cells, grid.y_cells, &left, &right, &bottom, &top);

  // Create the chunks
  chunk.task = parallel.task;

  x_cells = right - left + 1;
  y_cells = top - bottom + 1;

  chunk.left = left;
  chunk.bottom = bottom;
  chunk.right = right;
  chunk.top = top;
  chunk.left_boundary = 1;
  chunk.bottom_boundary = 1;
  chunk.right_boundary = grid.x_cells;
  chunk.top_boundary = grid.y_cells;
  chunk.x_min = 1;
  chunk.y_min = 1;
  chunk.x_max = x_cells;
  chunk.y_max = y_cells;

  // Create the tiles
  chunk.tiles = malloc(tiles_per_chunk * sizeof(tile_type));
  clover_tile_decompose(x_cells, y_cells);

  build_field();

  clover_allocate_buffers();

  if (parallel.boss)
    fputs("Generating chunks", g_out);

  for (int tile = 0; tile < tiles_per_chunk; tile++) {
    initialise_chunk(tile);
    // generate_chunk(tile);
  }

  advect_x = true;

  // Do no profile the start up costs otherwise the total times will not add up
  // at the end
  profiler_off = profiler_on;
  profiler_on = false;

  for (int tile = 0; tile < tiles_per_chunk; tile++)
    ideal_gas(&chunk.tiles[tile], false);

  memset(fields, 0, sizeof(fields));
  fields[FIELD_DENSITY0] = 1;
  fields[FIELD_ENERGY0] = 1;
  fields[FIELD_PRESSURE] = 1;
  fields[FIELD_VISCOSITY] = 1;
  fields[FIELD_DENSITY1] = 1;
  fields[FIELD_ENERGY1] = 1;
  fields[FIELD_XVEL0] = 1;
  fields[FIELD_YVEL0] = 1;
  fields[FIELD_XVEL1] = 1;
  fields[FIELD_YVEL1] = 1;

  // update_halo(fields, 2);

  if (parallel.boss)
    fputs("\nProblem initalised and generated", g_out);

  // field_summary();

  if (visit_frequency != 0)
    // visit();

    profiler_on = profiler_off;
}
