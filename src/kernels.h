// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

/**
 * @brief Bride code between clover and the kernels to allow for better separation of concerns. Avoids kernels accessing
 * global variables, which allows for easier testing.
 */

#pragma once

extern void initialise_chunk(int tile);

extern void generate_chunk(int tile);

extern void ideal_gas(int tile, bool predict);

extern void calc_dt(int tile, double *local_dt, char local_control[static 8], double *xl_pos, double *yl_pos, int *jldt,
                    int *kldt);
