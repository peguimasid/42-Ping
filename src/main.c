#include "../includes/ft-ping.h"

t_context *ctx;

int main(int argc, char **argv) {
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc < 2) {
    fatal_error(USAGE);
  }

  init();
  parse(argc, argv);
  setup_signal_handlers();
  ping();

  free(ctx);

  return EXIT_SUCCESS;
}

void fatal_error(char *errorstr) {
  printf(errorstr);
  if (ctx) free(ctx);
  exit(EXIT_FAILURE);
}
