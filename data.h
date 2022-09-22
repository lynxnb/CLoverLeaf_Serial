#pragma once

#include <bits/types/FILE.h>
#include "data_types.h"

#define g_version         1.3

#define g_ibig         640000

#define g_small       1.0e-16
#define g_big         1.0e+21

#define g_name_len_max    225
#define g_xdir              1
#define g_ydir              2

#define CHUNK_LEFT          1
#define CHUNK_RIGHT         2
#define CHUNK_BOTTOM        3
#define CHUNK_TOP           4
#define EXTERNAL_FACE      -1

#define TILE_LEFT           1
#define TILE_RIGHT          2
#define TILE_BOTTOM         3
#define TILE_TOP            4
#define EXTERNAL_TILE      -1

#define FIELD_DENSITY0      1
#define FIELD_DENSITY1      2
#define FIELD_ENERGY0       3
#define FIELD_ENERGY1       4
#define FIELD_PRESSURE      5
#define FIELD_VISCOSITY     6
#define FIELD_SOUNDSPEED    7
#define FIELD_XVEL0         8
#define FIELD_XVEL1         9
#define FIELD_YVEL0        10
#define FIELD_YVEL1        11
#define FIELD_VOL_FLUX_X   12
#define FIELD_VOL_FLUX_Y   13
#define FIELD_MASS_FLUX_X  14
#define FIELD_MASS_FLUX_Y  15
#define NUM_FIELDS         15

#define CELL_DATA           1
#define VERTEX_DATA         2
#define X_FACE_DATA         3
#define y_FACE_DATA         4

#define SOUND               1
#define X_VEL               2
#define Y_VEL               3
#define DIVERG              4

#define g_rect              1
#define g_circ              2
#define g_point             3

#define g_len_max         500

extern FILE *g_in, *g_out;

extern parallel_type parallel;
