/*Crown Copyright 2012 AWE.
 *
 * This file is part of CloverLeaf.
 *
 * CloverLeaf is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * CloverLeaf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CloverLeaf. If not, see http://www.gnu.org/licenses/. */

/**
 *  @brief Driver for chunk initialisation.
 *  @author Wayne Gaudin
 *  @details Invokes the user specified chunk initialisation kernel.
 */

#include "../types/definitions.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void kernel_initialise_chunk(tile_type *tile, double xmin, double ymin, double dx, double dy) {
  int x_min = tile->t_xmin;
  int x_max = tile->t_xmax;
  int y_min = tile->t_ymin;
  int y_max = tile->t_ymax;

  double *cellx = tile->field.cellx;
  double *celly = tile->field.celly;
  double *vertexx = tile->field.vertexx;
  double *vertexy = tile->field.vertexy;
  double *celldx = tile->field.celldx;
  double *celldy = tile->field.celldy;
  double *vertexdx = tile->field.vertexdx;
  double *vertexdy = tile->field.vertexdy;

  double *volume = tile->field.volume;
  double *xarea = tile->field.xarea;
  double *yarea = tile->field.yarea;

  int j, k;

#pragma ivdep
  for (j = 0; j <= (x_max + 3) + abs(x_min - 2); j++) {
    vertexx[j] = xmin + dx * (double)(j - x_min);
  }

#pragma ivdep
  for (j = x_min - 2; j <= x_max + 3; j++) {
    vertexdx[j] = dx;
  }

#pragma ivdep
  for (k = y_min - 2; k <= y_max + 3; k++) {
    vertexy[j] = ymin + dy * (double)(k - y_min);
  }

#pragma ivdep
  for (k = y_min - 2; k <= y_max + 3; k++) {
    vertexdy[j] = dy;
  }

#pragma ivdep
  for (j = x_min - 2; j <= x_max + 2; j++) {
    cellx[j] = 0.5 * (vertexx[j] + vertexx[j + 1]);
  }

#pragma ivdep
  for (j = x_min - 2; j <= x_max + 2; j++) {
    celldx[j] = dx;
  }

#pragma ivdep
  for (k = y_min - 2; k <= y_max + 2; k++) {
    celly[k] = 0.5 * (vertexy[k] + vertexy[k + 1]);
  }

#pragma ivdep
  for (k = y_min - 2; k <= y_max + 2; k++) {
    celldy[k] = dy;
  }

  int row_size = (y_max + 2) - (y_min - 2) + 1;
  for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 2; j++) {
      volume[k * row_size + j] = dx * dy;
    }
  }

  // row_size unchanged
  for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 2; j++) {
      xarea[k * row_size + j] = celldy[k];
    }
  }

  // row_size unchanged
  for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 2; j++) {
      yarea[k * row_size + j] = celldx[j];
    }
  }
}
