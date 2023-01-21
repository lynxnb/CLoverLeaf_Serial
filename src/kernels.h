// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

/**
 * @brief Bridge code between clover and the kernels to allow for better separation of concerns. Avoids kernels
 * accessing global variables, which allows for easier testing.
 */

#pragma once

#include <stdbool.h>

extern void initialise_chunk(int tile);

extern void generate_chunk(int tile);

extern void ideal_gas(int tile, bool predict);

extern void update_halo(int fields[static 15], int depth);

extern void field_summary();

extern void visit();

extern void viscosity();

extern void calc_dt(
    int tile, double *local_dt, char local_control[static 8], double *xl_pos, double *yl_pos, int *jldt, int *kldt
);

extern void PdV(bool predict);
