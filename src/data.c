// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "types/data.h"
#include <bits/types/FILE.h>
#include <stddef.h>

FILE *g_in = NULL;
FILE *g_out = NULL;

parallel_type parallel;
