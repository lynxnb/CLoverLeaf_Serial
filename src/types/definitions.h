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
} state_type;

typedef struct grid_type_t {
  double xmin;
  double ymin;
  double xmax;
  double ymax;
  int x_cells;
  int y_cells;
} grid_type;

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
} profiler_type;

typedef struct field_type_t {
  double *density0;
  double *density1;
  double *energy0;
  double *energy1;
  double *pressure;
  double *viscosity;
  double *soundspeed;
  double *xvel0;
  double *xvel1;
  double *yvel0;
  double *yvel1;
  double *vol_flux_x;
  double *mass_flux_x;
  double *vol_flux_y;
  double *mass_flux_y;
  double *work_array1; // node_flux, stepbymass, volume_change, pre_vol
  double *work_array2; // node_mass_post, post_vol
  double *work_array3; // node_mass_pre,pre_mass
  double *work_array4; // advec_vel, post_mass 
  double *work_array5; // mom_flux, advec_vol 
  double *work_array6; // pre_vol, post_ener
  double *work_array7; // post_vol, ener_flux

  double *cellx;
  double *celly;
  double *vertexx;
  double *vertexy;
  double *celldx;
  double *celldy;
  double *vertexdx;
  double *vertexdy;

  double *volume;
  double *xarea;
  double *yarea;
} field_type;

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
} tile_type;

typedef struct chunk_type_t {
  int task;
  int chunk_neighbours[4];

  double *left_rcv_buffer;
  double *right_rcv_buffer;
  double *bottom_rcv_buffer;
  double *top_rcv_buffer;

  double *left_snd_buffer;
  double *right_snd_buffer;
  double *bottom_snd_buffer;
  double *top_snd_buffer;

  tile_type *tiles; // allocatable

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
} chunk_type;
