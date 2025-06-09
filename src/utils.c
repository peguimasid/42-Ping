#include "../includes/ft-ping.h"

void fatal_error(char *errorstr) {
  printf(errorstr);
  if (ctx) free(ctx);
  exit(EXIT_FAILURE);
}
