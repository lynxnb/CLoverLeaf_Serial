// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "kernels/kernels.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "data.h"
#include "definitions.h"
#include "utils/timer.h"

void initialise_chunk(int tile) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  double dx = grid.xmax - grid.xmin / (double)grid.x_cells;
  double dy = grid.ymax - grid.ymin / (double)grid.y_cells;

  double xmin = grid.xmin + dx * (double)(tile_ptr->t_left - 1);
  double ymin = grid.ymin + dy * (double)(tile_ptr->t_bottom - 1);

  kernel_initialise_chunk(
      tile_ptr->t_xmin,
      tile_ptr->t_xmax,
      tile_ptr->t_ymin,
      tile_ptr->t_ymax,
      xmin,
      ymin,
      dx,
      dy,
      tile_ptr->field.vertexx,
      tile_ptr->field.vertexdx,
      tile_ptr->field.vertexy,
      tile_ptr->field.vertexdy,
      tile_ptr->field.cellx,
      tile_ptr->field.celldx,
      tile_ptr->field.celly,
      tile_ptr->field.celldy,
      tile_ptr->field.volume,
      tile_ptr->field.xarea,
      tile_ptr->field.yarea
  );
}

void generate_chunk(int tile) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  double state_density[number_of_states];
  double state_energy[number_of_states];
  double state_xvel[number_of_states];
  double state_yvel[number_of_states];
  double state_xmin[number_of_states];
  double state_xmax[number_of_states];
  double state_ymin[number_of_states];
  double state_ymax[number_of_states];
  double state_radius[number_of_states];
  int state_geometry[number_of_states];

  for (int state = 0; state <= number_of_states; state++) {
    state_density[state] = states[state].density;
    state_energy[state] = states[state].energy;
    state_xvel[state] = states[state].xvel;
    state_yvel[state] = states[state].yvel;
    state_xmin[state] = states[state].xmin;
    state_xmax[state] = states[state].xmax;
    state_ymin[state] = states[state].ymin;
    state_ymax[state] = states[state].ymax;
    state_radius[state] = states[state].radius;
    state_geometry[state] = states[state].geometry;
  }

  kernel_generate_chunk(
      tile_ptr->t_xmin,
      tile_ptr->t_xmax,
      tile_ptr->t_ymin,
      tile_ptr->t_ymax,
      tile_ptr->field.vertexx,
      tile_ptr->field.vertexy,
      tile_ptr->field.cellx,
      tile_ptr->field.celly,
      tile_ptr->field.density0,
      tile_ptr->field.energy0,
      tile_ptr->field.xvel0,
      tile_ptr->field.yvel0,
      number_of_states,
      state_density,
      state_energy,
      state_xvel,
      state_yvel,
      state_xmin,
      state_xmax,
      state_ymin,
      state_ymax,
      state_radius,
      state_geometry
  );
}

void ideal_gas(int tile, bool predict) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  kernel_ideal_gas(
      tile_ptr->t_xmin,
      tile_ptr->t_xmax,
      tile_ptr->t_ymin,
      tile_ptr->t_ymax,
      predict ? tile_ptr->field.density1 : tile_ptr->field.density0,
      predict ? tile_ptr->field.energy1 : tile_ptr->field.energy0,
      tile_ptr->field.pressure,
      tile_ptr->field.soundspeed
  );
}

void update_tile_halo(int fields[static 15], int depth) {}

void update_halo(int fields[static 15], int depth) {
  double kernel_time;

  if (profiler_on)
    kernel_time = timer();

  update_tile_halo(fields, depth);

  if (profiler_on) {
    profiler.tile_halo_exchange += timer() - kernel_time;
    kernel_time = timer();

    if (chunk.chunk_neighbours[CHUNK_LEFT] == EXTERNAL_FACE || chunk.chunk_neighbours[CHUNK_RIGHT] == EXTERNAL_FACE ||
        chunk.chunk_neighbours[CHUNK_BOTTOM] == EXTERNAL_FACE || chunk.chunk_neighbours[CHUNK_TOP] == EXTERNAL_FACE) {
      for (int tile = 0; tile < tiles_per_chunk; tile++) {
        tile_type *cur_tile = &chunk.tiles[tile];

        kernel_update_halo(
            cur_tile->t_xmin,
            cur_tile->t_xmax,
            cur_tile->t_ymin,
            cur_tile->t_ymax,
            chunk.chunk_neighbours,
            cur_tile->tile_neighbours,
            cur_tile->field.density0,
            cur_tile->field.energy0,
            cur_tile->field.pressure,
            cur_tile->field.viscosity,
            cur_tile->field.soundspeed,
            cur_tile->field.density1,
            cur_tile->field.energy1,
            cur_tile->field.xvel0,
            cur_tile->field.yvel0,
            cur_tile->field.xvel1,
            cur_tile->field.yvel1,
            cur_tile->field.vol_flux_x,
            cur_tile->field.vol_flux_y,
            cur_tile->field.mass_flux_x,
            cur_tile->field.mass_flux_y,
            fields,
            depth
        );
      }
    }

    if (profiler_on)
      profiler.self_halo_exchange += timer() - kernel_time;
  }
}

void field_summary() {
  double vol, mass, ie, ke, press;
  double t_vol, t_mass, t_ie, t_ke, t_press;
  double qa_diff;

  double kernel_time;

  if (parallel.boss) {
    fprintf(g_out, "\nTime %ld\n", time(NULL));
    // Print table header row with column width 16
    fprintf(
        g_out,
        "             %-22s %-22s %-22s %-22s %-22s %-22s %-22s",
        "Volume",
        "Mass",
        "Density",
        "Pressure",
        "Internal Energy",
        "Kinetic Energy",
        "Total Energy"
    );
  }

  if (profiler_on) {
    kernel_time = timer();
  }

  for (int tile = 0; tile < tiles_per_chunk; tile++) ideal_gas(tile, false);

  if (profiler_on) {
    profiler.ideal_gas += timer() - kernel_time;
    kernel_time = timer();
  }

  t_vol = 0.0;
  t_mass = 0.0;
  t_ie = 0.0;
  t_ke = 0.0;
  t_press = 0.0;

  for (int tile = 0; tile < tiles_per_chunk; tile++) {
    tile_type *cur_tile = &chunk.tiles[tile];

    kernel_field_summary(
        chunk.tiles[tile].t_xmin,
        cur_tile->t_xmax,
        cur_tile->t_ymin,
        cur_tile->t_ymax,
        cur_tile->field.volume,
        cur_tile->field.density0,
        cur_tile->field.energy0,
        cur_tile->field.pressure,
        cur_tile->field.xvel0,
        cur_tile->field.yvel0,
        &vol,
        &mass,
        &ie,
        &ke,
        &press
    );

    t_vol += vol;
    t_mass += mass;
    t_ie += ie;
    t_ke += ke;
    t_press += press;
  }

  if (profiler_on)
    profiler.summary += timer() - kernel_time;

  if (parallel.boss) {
    fprintf(
        g_out,
        " step:%7d %22.4e %22.4e %22.4e %22.4e %22.4e %22.4e %22.4e\n",
        step,
        t_vol,
        t_mass,
        t_mass / t_vol,
        t_press / t_vol,
        t_ie,
        t_ke,
        t_ie + t_ke
    );
  }

  if (complete && parallel.boss && test_problem >= 1) {
    double ke_constant;
    switch (test_problem) {
      case 1:
        ke_constant = 1.82280367310258;
        break;
      case 2:
        ke_constant = 1.19316898756307;
        break;
      case 3:
        ke_constant = 2.58984003503994;
        break;
      case 4:
        ke_constant = 0.307475452287895;
        break;
      case 5:
        ke_constant = 4.85350315783719;
        break;
      default:
        ke_constant = 1.0;
        break;
    }

    qa_diff = fabs((100.0 * (t_ke / ke_constant)) - 100.0);

    printf("Test problem %4d is within %16.7e%% of the expected solution\n", test_problem, qa_diff);
    fprintf(g_out, "Test problem %4d is within %16.7e%% of the expected solution\n", test_problem, qa_diff);

    if (qa_diff < 0.001) {
      puts("This test is considered PASSED");
      fputs("This test is considered PASSED", g_out);
    } else {
      puts("This test is considered NOT PASSED");
      fputs("This test is considered NOT PASSED", g_out);
    }
  }
}

void visit() {
  // In the original source, this kernel would have printed data to a .vtk file
  // However, since max_task was always equal to 1, the code of this kernel was never actually executed
  // For this reason, the implementation of it has been skipped
}

void calc_dt(
    int tile, double *local_dt, char local_control[static 8], double *xl_pos, double *yl_pos, int *jldt, int *kldt
) {
  tile_type *tile_ptr = &chunk.tiles[tile];
  int small = 0, l_control;
  *local_dt = G_BIG;

  // kernel_calc_dt();

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