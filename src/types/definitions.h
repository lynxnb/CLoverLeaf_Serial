// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include <stdbool.h>

typedef struct state_type_t {
  bool defined;
  double density, energy, xvel, yvel;
  int geometry;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double radius;
} state_type; // 88 bytes

typedef struct grid_type_t {
  double xmin;
  double ymin;
  double xmax;
  double ymax;
  int x_cells;
  int y_cells;
} grid_type; // 40 bytes

typedef struct profiler_type_t {
  double timestep;
  double acceleration;
  double PdV;
  double cell_advection;
  double mom_advection;
  double viscosity;
  double ideal_gas;
  double visit;
  double summary;
  double reset;
  double revert;
  double flux;
  double tile_halo_exchange;
  double self_halo_exchange;
  double mpi_halo_exchange;
} profiler_type; // 120 bytes

typedef struct field_type_t {
  double *density0;     // 2D array
  double *density1;     // 2D array
  double *energy0;      // 2D array
  double *energy1;      // 2D array
  double *pressure;     // 2D array
  double *viscosity;    // 2D array
  double *soundspeed;   // 2D array
  double *xvel0;        // 2D array
  double *xvel1;        // 2D array
  double *yvel0;        // 2D array
  double *yvel1;        // 2D array
  double *vol_flux_x;   // 2D array
  double *mass_flux_x;  // 2D array
  double *vol_flux_y;   // 2D array
  double *mass_flux_y;  // 2D array
  double *work_array1;  // 2D array | node_flux, stepbymass, volume_change, pre_vol
  double *work_array2;  // 2D array | node_mass_post, post_vol
  double *work_array3;  // 2D array | node_mass_pre, pre_mass
  double *work_array4;  // 2D array | advec_vel, post_mass
  double *work_array5;  // 2D array | mom_flux, advec_vol
  double *work_array6;  // 2D array | pre_vol, post_ener
  double *work_array7;  // 2D array | post_vol, ener_flux

  double *cellx;     // 1D array
  double *celly;     // 1D array
  double *vertexx;   // 1D array
  double *vertexy;   // 1D array
  double *celldx;    // 1D array
  double *celldy;    // 1D array
  double *vertexdx;  // 1D array
  double *vertexdy;  // 1D array

  double *volume;  // 2D array
  double *xarea;   // 2D array
  double *yarea;   // 2D array
} field_type; // 264 bytes

typedef struct tile_type_t {
  field_type field;
  int tile_neighbours[4];
  int external_tile_mask[4];

  int t_xmin;
  int t_xmax;
  int t_ymin;
  int t_ymax;

  int t_left;
  int t_right;
  int t_bottom;
  int t_top;
} tile_type; // 328 bytes

typedef struct chunk_type_t {
  int task;
  int chunk_neighbours[4];

  tile_type *tiles;  // 1D array

  int x_min;
  int y_min;
  int x_max;
  int y_max;

  int left;
  int right;
  int bottom;
  int top;
  int left_boundary;
  int right_boundary;
  int bottom_boundary;
  int top_boundary;
} chunk_type; // 80 bytes
