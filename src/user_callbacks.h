// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

/*
 * Collection of functions that can be used to run custom code at specific execution points in the program
 * execution.
 */

#pragma once

extern void hydro_init();

extern void hydro_foreach_step(int step);

extern void hydro_done();
