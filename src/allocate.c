// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "definitions.h"

#include <stdlib.h>

/**
 * @brief Allocates the data for each mesh chunk
 * @details The data fields for the mesh chunk are allocated based on the mesh size
 */
void build_field() {
  int tile, j, k, alloc_size;

  for (tile = 0; tile < tiles_per_chunk; tile++) {
    tile_type *cur_tile = &chunk.tiles[tile];
    field_type *cur_field = &cur_tile->field;

    alloc_size = (cur_tile->t_xmax + 2) - (cur_tile->t_xmin - 2) + 1;
    cur_field->cellx = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_ymax + 2) - (cur_tile->t_ymin - 2) + 1;
    cur_field->celly = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_xmax + 3) - (cur_tile->t_xmin - 2) + 1;
    cur_field->vertexx = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_ymax + 3) - (cur_tile->t_ymin - 2) + 1;
    cur_field->vertexy = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_xmax + 2) - (cur_tile->t_xmin - 2) + 1;
    cur_field->celldx = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_ymax + 2) - (cur_tile->t_ymin - 2) + 1;
    cur_field->celldy = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_xmax + 3) - (cur_tile->t_xmin - 2) + 1;
    cur_field->vertexdx = malloc(alloc_size * sizeof(double));

    alloc_size = (cur_tile->t_ymax + 3) - (cur_tile->t_ymin - 2) + 1;
    cur_field->vertexdy = malloc(alloc_size * sizeof(double));

    // Zeroing isn't strictly neccessary but it ensures physical pages
    // are allocated. This prevents first touch overheads in the main code
    // cycle which can skew timings in the first step

    for (j = 0; j <= (cur_tile->t_xmax + 2) - (cur_tile->t_xmin - 2); j++) {
      cur_field->cellx[j] = 0.0;
      cur_field->celldx[j] = 0.0;
    }

    for (j = 0; j <= (cur_tile->t_ymax + 2) - (cur_tile->t_ymin - 2); j++) {
      cur_field->celly[j] = 0.0;
      cur_field->celldy[j] = 0.0;
    }

    for (j = 0; j <= (cur_tile->t_xmax + 3) - (cur_tile->t_xmin - 2); j++) {
      cur_field->vertexx[j] = 0.0;
      cur_field->vertexdx[j] = 0.0;
    }

    for (j = 0; j <= (cur_tile->t_ymax + 3) - (cur_tile->t_ymin - 2); j++) {
      cur_field->vertexy[j] = 0.0;
      cur_field->vertexdy[j] = 0.0;
    }
  }
}