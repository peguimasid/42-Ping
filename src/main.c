#include "../includes/ft-ping.h"

void parse_args(t_context *ctx, int argc, char **argv) {
  (void)ctx;
  (void)argc;
  (void)argv;
}

int main(int argc, char **argv) {
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc != 2) {
    fatal_error("Usage: ft_ping [-v verbose] [-h help] hostname");
  }

  t_context *ctx = init_context();
  parse_args(ctx, argc, argv);
  free(ctx);

  return EXIT_SUCCESS;
}
