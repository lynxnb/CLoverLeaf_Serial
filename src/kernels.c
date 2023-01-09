// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "kernels/kernels.h"
#include "data.h"
#include "definitions.h"
#include <string.h>

void initialise_chunk(int tile) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  double dx = grid.xmax - grid.xmin / (double)grid.x_cells;
  double dy = grid.ymax - grid.ymin / (double)grid.y_cells;

  double xmin = grid.xmin + dx * (double)(tile_ptr->t_left - 1);
  double ymin = grid.ymin + dy * (double)(tile_ptr->t_bottom - 1);

  kernel_initialise_chunk(tile_ptr, xmin, ymin, dx, dy);
}

void generate_chunk(int tile) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  double state_density[number_of_states], state_energy[number_of_states], state_xvel[number_of_states],
      state_yvel[number_of_states], state_xmin[number_of_states], state_xmax[number_of_states],
      state_ymin[number_of_states], state_ymax[number_of_states], state_radius[number_of_states];
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

  kernel_generate_chunk(tile_ptr, number_of_states, state_density, state_energy, state_xvel, state_yvel, state_xmin,
                        state_xmax, state_ymin, state_ymax, state_radius, state_geometry);
}

void ideal_gas(int tile, bool predict) {
  tile_type *tile_ptr = &chunk.tiles[tile];

  kernel_ideal_gas(tile_ptr, predict);
}

void calc_dt(int tile, double *local_dt, char local_control[static 8], double *xl_pos, double *yl_pos, int *jldt,
             int *kldt) {
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
