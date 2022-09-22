// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data_types.h"
#include <bits/types/FILE.h>
#include <stddef.h>

FILE *g_in = NULL,
     *g_out = NULL;

parallel_type parallel;
