#include "data.h"
#include "definitions.h"
#include "parse.h"
#include "read_input.h"
#include "report.h"
#include <errno.h>
#include <stdio.h>

void initialise() {
  FILE *uin = NULL, *out_unit = NULL;

  if (parallel.boss) {
    errno = 0;
    g_out = fopen("clover.out", "w");
    if (errno != 0) {
      g_out = NULL;
      report_error("initialise", "Error opening clover.out file.");
    }

    fprintf(g_out, "Clover Version %f\nMPI Version\nTask Count %d\n", g_version,
            parallel.max_task);

    puts("Output file clover.out opened. All output will go there.");

    fputs("\nClover will run from the following input:-\n", g_out);

    errno = 0;
    uin = fopen("clover.in", "r");
    if (errno != 0) {
      errno = 0;
      out_unit = fopen("clover.in", "w");
      if (errno != 0)
        report_error("initialise", "Error opening clover.in file");

      fputs("*clover\n"
            " state 1 density=0.2 energy=1.0\n"
            " state 2 density=1.0 energy=2.5 geometry=rectangle xmin=0.0"
            " xmax=5.0 ymin=0.0 ymax=2.0\n"
            " x_cells=10\n"
            " y_cells=2\n"
            " xmin=0.0\n"
            " ymin=0.0\n"
            " xmax=10.0\n"
            " ymax=2.0\n"
            " initial_timestep=0.04\n"
            " timestep_rise=1.5\n"
            " max_timestep=0.04\n"
            " end_time=3.0\n"
            " test_problem 1\n"
            "*endclover\n",
            out_unit);

      fclose(out_unit);
      uin = fopen("clover.in", "r");
    }

    errno = 0;
    out_unit = fopen("clover.in.tmp", "w");
    if (errno != 0)
      report_error("initialise", "Error opening clover.in.tmp file");

    int stat = parse_init(uin, "");
    while (true) {
      stat = parse_getline(-1);
      if (stat != 0)
        break;
      fputs(line, out_unit);
      fputc('\n', out_unit);
    }
    fclose(out_unit);
  }

  errno = 0;
  g_in = fopen("clover.in.tmp", "r");
  if (errno != 0)
    report_error("initialise", "Error opening clover.in.tmp file");

  if (parallel.boss) {
    // Write input file to output file
    rewind(uin);
    char buf[100];
    while (true) {
      if (fgets(buf, 100, uin) == NULL)
        break;
      fputs(buf, g_out);
    }

    fputs("\nInitialising and generating\n\n", g_out);
  }

  read_input();

  step = 0;

  if (parallel.boss)
    fputs("Starting the calculation", g_out);

  fclose(g_in);
}
