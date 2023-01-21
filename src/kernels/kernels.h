// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include <stdbool.h>

extern void kernel_initialise_chunk(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double min_x,
    double min_y,
    double d_x,
    double d_y,
    double *vertexx,
    double *vertexdx,
    double *vertexy,
    double *vertexdy,
    double *cellx,
    double *celldx,
    double *celly,
    double *celldy,
    double *volume,
    double *xarea,
    double *yarea
);

extern void kernel_generate_chunk(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double *vertexx,
    double *vertexy,
    double *cellx,
    double *celly,
    double *density0,
    double *energy0,
    double *xvel0,
    double *yvel0,
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

extern void kernel_ideal_gas(
    int x_min, int x_max, int y_min, int y_max, double *density, double *energy, double *pressure, double *soundspeed
);

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

extern void kernel_viscosity(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double *celldx,
    double *celldy,
    double *density0,
    double *pressure,
    double *viscosity,
    double *xvel0,
    double *yvel0
);

extern void kernel_calc_dt(
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double min_dt,
    double dtc_safe,
    double dtu_safe,
    double dtv_safe,
    double dtdiv_safe,
    double *xarea,
    double *yarea,
    double *cellx,
    double *celly,
    double *celldx,
    double *celldy,
    double *volume,
    double *density0,
    double *energy0,
    double *pressure,
    double *viscosity,
    double *soundspeed,
    double *xvel0,
    double *yvel0,
    double *dt_min,
    double *dtminval,
    int *dtlcontrol,
    double *xlpos,
    double *ylpos,
    int *jldt,
    int *kldt,
    int *smll
);

extern void kernel_pdv(
    bool predict,
    int x_min,
    int x_max,
    int y_min,
    int y_max,
    double dt,
    double *xarea,
    double *yarea,
    double *volume,
    double *density0,
    double *density1,
    double *energy0,
    double *energy1,
    double *pressure,
    double *viscosity,
    double *xvel0,
    double *xvel1,
    double *yvel0,
    double *yvel1,
    double *volume_change
);
