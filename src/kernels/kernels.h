// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include "../types/definitions.h"

extern void kernel_initialise_chunk(tile_type *tile, double xmin, double ymin, double dx, double dy);

extern void kernel_generate_chunk(tile_type *tile, int number_of_states, double *state_density, double *state_energy,
                                  double *state_xvel, double *state_yvel, double *state_xmin, double *state_xmax,
                                  double *state_ymin, double *state_ymax, double *state_radius, int *state_geometry);

extern void kernel_ideal_gas(tile_type *tile, bool predict);

extern void kernel_calc_dt(tile_type *tile, double local_dt, char local_control[static 8], double xl_pos, double yl_pos,
                           int jldt, int kldt);
