// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include "../types/definitions.h"

extern void kernel_initialise_chunk(tile_type *tile, double xmin, double ymin, double dx, double dy);

extern void kernel_generate_chunk(
    tile_type *tile,
    int number_of_states,
    double *state_density,
    double *state_energy,
    double *state_xvel,
    double *state_yvel,
    double *state_xmin,
    double *state_xmax,
    double *state_ymin,
    double *state_ymax,
    double *state_radius,
    int *state_geometry
);

extern void kernel_ideal_gas(tile_type *tile, bool predict);

extern void kernel_update_tile_halo();

extern void kernel_update_halo(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    int chunk_neighbours[static 4],
    int tile_neighbours[static 4],
    double *density0,
    double *energy0,
    double *pressure,
    double *viscosity,
    double *soundspeed,
    double *density1,
    double *energy1,
    double *xvel0,
    double *yvel0,
    double *xvel1,
    double *yvel1,
    double *vol_flux_x,
    double *vol_flux_y,
    double *mass_flux_x,
    double *mass_flux_y,
    int fields[static 15],
    int depth
);

extern void kernel_field_summary(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double *volume,
    double *density0,
    double *energy0,
    double *pressure,
    double *xvel0,
    double *yvel0,
    double *vol,
    double *mass,
    double *ie,
    double *ke,
    double *press
);

extern void kernel_calc_dt(
    tile_type *tile, double local_dt, char local_control[static 8], double xl_pos, double yl_pos, int jldt, int kldt
);
