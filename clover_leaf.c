#include "clover.h"
#include "data.h"
#include "hydro.h"
#include "initialise.h"
#include <stdio.h>

int main(int argc, char **argv) {
  clover_init_comms();

  printf("Clover Version %f\nMPI Version\nTask Count %d\n", g_version,
         parallel.max_task);

  initialise();

  hydro();
}
