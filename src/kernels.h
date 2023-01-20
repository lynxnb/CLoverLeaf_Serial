// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

/**
 * @brief Bridge code between clover and the kernels to allow for better separation of concerns. Avoids kernels
 * accessing global variables, which allows for easier testing.
 */

#pragma once

#include <stdbool.h>

#ifdef NO_INLINE_KERNELS
#define KERNEL_DECL extern
#else
#define KERNEL_DECL static inline
#endif

KERNEL_DECL void initialise_chunk(int tile);

KERNEL_DECL void generate_chunk(int tile);

KERNEL_DECL void ideal_gas(int tile, bool predict);

KERNEL_DECL void update_halo(int fields[static 15], int depth);

KERNEL_DECL void field_summary();

KERNEL_DECL void visit();

KERNEL_DECL void calc_dt(
    int tile, double *local_dt, char local_control[static 8], double *xl_pos, double *yl_pos, int *jldt, int *kldt
);

#undef KERNEL_DECL
