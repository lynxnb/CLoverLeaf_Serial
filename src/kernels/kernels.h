// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include "../types/definitions.h"

extern void initialise_chunk(int tile);

extern void ideal_gas(tile_type *tile, bool predict);

extern void calc_dt(int tile, double local_dt, char local_control[static 8], double xl_pos, double yl_pos, int jldt,
             int kldt);
