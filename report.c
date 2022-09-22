#include "clover.h"
#include "data.h"
#include <stdio.h>

void report_error_arg(const char *location, const char *error, const char *arg) {
  const char *format = "\nError in %s: %s%s\n\nCLOVER is terminating.\n";

  fprintf(stdout, format, location, error, arg);
  if (g_out != NULL)
    fprintf(g_out, format, location, error, arg);

  clover_abort();
}

void report_error(const char *location, const char *error) {
  report_error_arg(location, error, "");
}
