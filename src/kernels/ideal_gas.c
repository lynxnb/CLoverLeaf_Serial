/*Crown Copyright 2012 AWE.
 *
 * This file is part of CloverLeaf.
 *
 * CloverLeaf is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * CloverLeaf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CloverLeaf. If not, see http://www.gnu.org/licenses/. */

/**
 *  @brief C ideal gas kernel.
 *  @author Wayne Gaudin
 *  @details Calculates the pressure and sound speed for the mesh chunk using
 *  the ideal gas equation of state, with a fixed gamma of 1.4.
 */

#include "../types/definitions.h"
#include "ftocmacros.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void ideal_gas(tile_type *cur_tile, bool predict) {
  int x_min = cur_tile->t_xmin;
  int x_max = cur_tile->t_xmax;
  int y_min = cur_tile->t_ymin;
  int y_max = cur_tile->t_ymax;
  double *density, *energy;

  if (predict) {
    density = cur_tile->field.density1;
    energy = cur_tile->field.energy1;
  } else {
    density = cur_tile->field.density0;
    energy = cur_tile->field.energy0;
  }

  double *pressure = cur_tile->field.pressure;
  double *soundspeed = cur_tile->field.soundspeed;

  int j, k;
  double sound_speed_squared, v, pressurebyenergy, pressurebyvolume;

  for (k = y_min; k <= y_max; k++) {
#pragma ivdep
    for (j = x_min; j <= x_max; j++) {
      // (j, k, x_max + 4, x_min - 2, y_min - 2) -> ((x_max + 4) * (k - (y_min - 2)) + (j) - (x_min - 2))
      // (1, 1, 4 + 4    , 0 - 2    , 0 - 2    ) -> ((8        ) * (1 - (-2       )) + (1) - (-2       ))
      v = 1.0 / density[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)];
      pressure[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)] =
          (1.4 - 1.0) * density[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)] *
          energy[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)];
      pressurebyenergy = (1.4 - 1.0) * density[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)];
      pressurebyvolume = -density[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)] *
                         pressure[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)];
      sound_speed_squared =
          v * v * (pressure[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)] * pressurebyenergy - pressurebyvolume);
      soundspeed[FTNREF2D(j, k, x_max + 4, x_min - 2, y_min - 2)] = sqrt(sound_speed_squared);
    }
  }
}
