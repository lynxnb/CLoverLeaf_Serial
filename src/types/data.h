// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#pragma once

#include <stdbool.h>

typedef struct parallel_type_t {
  bool parallel;
  bool boss;
  int max_task;
  int task;
  int boss_task;
} parallel_type;
