/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#include <stdio.h>
#include <stdlib.h>
#include "cli.h"

static const char *const usage_message =
  "MC514 - Lab1\n"
  "Raphael Kubo da Costa, RA 072201\n"
  "\n"
  "Usage: camp <NUMTHREADS>\n"
  "\n"
  "  NUMTHREADS is the number of threads the program must create\n";

size_t cli_get_thread_count(int argc, char *argv[])
{
  char *error;
  long count;

  if (argc != CLI_ARGC)
    return 0;

  count = strtol(argv[1], &error, 10);
  if ((*error != '\0') || (count <= 0)) {
    fprintf(stderr, "Error: invalid thread number.\n");
    return 0;
  }

  return (size_t) count;
}

void cli_show_usage(void)
{
  fprintf(stderr, usage_message);
}
