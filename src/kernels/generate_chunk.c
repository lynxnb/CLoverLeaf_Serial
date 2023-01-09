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
 *  @brief C mesh chunk generator
 *  @author Wayne Gaudin
 *  @details Generates the field data on a mesh chunk based on the user specified
 *  input for the states.
 *
 *  Note that state one is always used as the background state, which is then
 *  overwritten by further state definitions.
 */

#include "../types/data.h"
#include "../types/definitions.h"
#include "../utils/array_indexing.h"
#include <math.h>

void kernel_generate_chunk(tile_type *tile, int number_of_states, double *state_density, double *state_energy,
                           double *state_xvel, double *state_yvel, double *state_xmin, double *state_xmax,
                           double *state_ymin, double *state_ymax, double *state_radius, int *state_geometry) {
  int x_min = tile->t_xmin;
  int x_max = tile->t_xmax;
  int y_min = tile->t_ymin;
  int y_max = tile->t_ymax;

  double *vertexx = tile->field.vertexx;
  double *vertexy = tile->field.vertexy;
  double *cellx = tile->field.cellx;
  double *celly = tile->field.celly;
  double *density0 = tile->field.density0;
  double *energy0 = tile->field.energy0;
  double *xvel0 = tile->field.xvel0;
  double *yvel0 = tile->field.yvel0;

  double radius, x_cent, y_cent;
  int state;

  int j, k, jt, kt;

  /* State 1 is always the background state */

  int row_size = (y_max + 2) - (y_min - 2) + 1;
  for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 2; j++) {
      energy0[INDEX2D(k, j, row_size)] = state_energy[0];
    }
  }

  // row size unchanged
  for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 2; j++) {
      density0[INDEX2D(k, j, row_size)] = state_density[0];
    }
  }

  row_size = (y_max + 3) - (y_min - 2) + 1;
  for (k = y_min - 2; k <= y_max + 3; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 3; j++) {
      xvel0[INDEX2D(k, j, row_size)] = state_xvel[0];
    }
  }

  // row size unchanged
  for (k = y_min - 2; k <= y_max + 3; k++) {
#pragma ivdep
    for (j = x_min - 2; j <= x_max + 3; j++) {
      yvel0[INDEX2D(k, j, row_size)] = state_yvel[0];
    }
  }

  int row_size_outer = (y_max + 2) - (y_min - 2) + 1; // energy0, density0
  int row_size_inner = (y_max + 3) - (y_min - 2) + 1; // xvel0, yvel0
  for (state = 1; state <= number_of_states; state++) {

    /* Could the velocity setting be thread unsafe? */
    x_cent = state_xmin[state];
    y_cent = state_ymin[state];

    for (k = y_min - 2; k <= y_max + 2; k++) {
#pragma ivdep
      for (j = x_min - 2; j <= x_max + 2; j++) {
        bool geometry_condition = false;

        if (state_geometry[state] == G_RECT) {
          geometry_condition = (vertexx[j + 1] >= state_xmin[state] && vertexx[j] < state_xmax[state]) &&
                               (vertexy[k + 1] >= state_ymin[state] && vertexy[k] < state_ymax[state]);
        } else if (state_geometry[state] == G_CIRC) {
          radius = sqrt((cellx[j] - x_cent) * (cellx[j] - x_cent) + (celly[k] - y_cent) * (celly[j] - y_cent));
          geometry_condition = radius <= state_radius[state];
        } else if (state_geometry[state] == G_POINT) {
          geometry_condition = (vertexx[j] == x_cent && vertexy[j] == y_cent);
        }

        if (geometry_condition) {
          energy0[INDEX2D(k, j, row_size_outer)] = state_energy[state];
          density0[INDEX2D(k, j, row_size_outer)] = state_density[state];

          for (kt = k; kt <= k + 1; kt++) {
            for (jt = j; jt <= j + 1; jt++) {
              xvel0[INDEX2D(k, j, row_size_inner)] = state_xvel[state];
              yvel0[INDEX2D(k, j, row_size_inner)] = state_yvel[state];
            }
          }
        }
      }
    }
  }
}
