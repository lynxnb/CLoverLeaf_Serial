// SPDX-License-Identifier: MIT
// Copyright (C) 2022 Niccolò Betto

#include "array.h"
#include <stddef.h>

#define SHIFT_PTR_1D(ptr, lower_bound) ptr - lower_bound

int *array_shift_indexing_1D_int(int *ptr, size_t lower_bound) {
  return SHIFT_PTR_1D(ptr, lower_bound);
}

float *array_shift_indexing_1D_float(float *ptr, size_t lower_bound) {
  return SHIFT_PTR_1D(ptr, lower_bound);
}

double *array_shift_indexing_1D_double(double *ptr, size_t lower_bound) {
  return SHIFT_PTR_1D(ptr, lower_bound);
}

#define ROW_SIZE(lower_bound_x, upper_bound_x) (upper_bound_x - lower_bound_x + 1)
#define SHIFT_PTR_2D(ptr, lower_bound_y, lower_bound_x, upper_bound_x)                                                 \
  ptr - lower_bound_x *ROW_SIZE(lower_bound_x, upper_bound_x) - lower_bound_y

int *array_shift_indexing_2D_int(int *ptr, size_t lower_bound_y, size_t lower_bound_x, size_t upper_bound_x) {
  return SHIFT_PTR_2D(ptr, lower_bound_y, lower_bound_x, upper_bound_x);
}

float *array_shift_indexing_2D_float(float *ptr, size_t lower_bound_y, size_t lower_bound_x, size_t upper_bound_x) {
  return SHIFT_PTR_2D(ptr, lower_bound_y, lower_bound_x, upper_bound_x);
}

double *array_shift_indexing_2D_double(double *ptr, size_t lower_bound_y, size_t lower_bound_x, size_t upper_bound_x) {
  return SHIFT_PTR_2D(ptr, lower_bound_y, lower_bound_x, upper_bound_x);
}
