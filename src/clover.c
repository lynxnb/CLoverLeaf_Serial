// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2022 Niccolò Betto

#include "data.h"
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>

void clover_init_comms() {
  const int rank = 0, size = 1;

  parallel.parallel = true;
  parallel.task = rank;

  if (rank == 0)
    parallel.boss = true;

  parallel.boss_task = 0;
  parallel.max_task = size;
}

void clover_abort() {
  if (g_in != NULL)
    fclose(g_in);

  if (g_out != NULL)
    fclose(g_out);

  exit(1);
}

int clover_get_num_chunks() {
  return parallel.max_task;
}

void clover_decompose(int x_cells, int y_cells, int *left, int *right, int *bottom, int *top) {
  // This decomposes the mesh into a number of chunks.
  // The number of chunks may be a multiple of the number of mpi tasks
  // Doesn't always return the best split if there are few factors
  // All factors need to be stored and the best picked. But its ok for now

  int delta_x, delta_y;

  double mesh_ratio, factor_x, factor_y;
  int chunk_x, chunk_y, mod_x, mod_y, split_found;

  int cx, cy, cnk, add_x, add_y, add_x_prev, add_y_prev;

  // 2D decomposition of the mesh

  mesh_ratio = (double)x_cells / (double)y_cells;

  chunk_x = number_of_chunks;
  chunk_y = 1;

  split_found = 0; // Used to detect 1D decomposition

  for (int c = 0; c < number_of_chunks; c++) {
    if (number_of_chunks % c == 0) {
      factor_x = number_of_chunks / (double)c;
      factor_y = c;
      // Compare the factor ration with the mesh ratio
      if (factor_x / factor_y <= mesh_ratio) {
        chunk_y = c;
        chunk_x = number_of_chunks / c;
        split_found = 1;
      }
    }
  }

  if (split_found == 0 || chunk_y == number_of_chunks) {
    // Prime number or 1D decomp detected
    if (mesh_ratio >= 1.0) {
      chunk_x = number_of_chunks;
      chunk_y = 1;
    } else {
      chunk_x = 1;
      chunk_y = number_of_chunks;
    }
  }

  delta_x = x_cells / chunk_x;
  delta_y = y_cells / chunk_y;
  mod_x = x_cells % chunk_x;
  mod_y = y_cells % chunk_y;

  // Set up chunk mesh ranges and chunk connectivity

  add_x_prev = 0;
  add_y_prev = 0;
  cnk = 1;

  for (int cx = 0; cx < chunk_x; cx++) {
    for (int cy = 0; cy < chunk_y; cy++) {
      add_x = 0;
      add_y = 0;
      if (cx <= mod_x)
        add_x = 1;
      if (cy <= mod_y)
        add_y = 1;

      if (cnk == parallel.task - 1) {
        left = (cx - 1) * delta_x + 1 + add_x_prev;
        right = left + delta_x - 1 + add_x;
        bottom = (cy - 1) * delta_y + 1 + add_y_prev;
        top = bottom + delta_y - 1 + add_y;

        chunk.chunk_neighbours[CHUNK_LEFT] = chunk_x * (cy - 1) + cx - 1;
        chunk.chunk_neighbours[CHUNK_RIGHT] = chunk_x * (cy - 1) + cx + 1;
        chunk.chunk_neighbours[CHUNK_BOTTOM] = chunk_x * (cy - 2) + cx;
        chunk.chunk_neighbours[CHUNK_TOP] = chunk_x * cy + cx;

        if (cx == 1)
          chunk.chunk_neighbours[CHUNK_LEFT] = EXTERNAL_FACE;
        if (cx == chunk_x)
          chunk.chunk_neighbours[CHUNK_RIGHT] = EXTERNAL_FACE;
        if (cy == 1)
          chunk.chunk_neighbours[CHUNK_BOTTOM] = EXTERNAL_FACE;
        if (cy == chunk_y)
          chunk.chunk_neighbours[CHUNK_TOP] = EXTERNAL_FACE;
      }

      if (cy <= mod_y)
        add_y_prev++;
      cnk++;
    }
    add_y_prev = 0;
    if (cx <= mod_x)
      add_x_prev++;
  }

  if (parallel.boss) {
    fprintf(g_out, "\nMesh ratio of %lf", mesh_ratio);
    fprintf(g_out, "Decomposing the mesh into %d by %d chunks\n", chunk_x, chunk_y);
    fprintf(g_out, "Decomposing the chunk with %d tiles\n", tiles_per_chunk);
  }
}

void clover_tile_decompose(int chunk_x_cells, int chunk_y_cells) {}

void clover_min(double dt) {
  // empty
}
