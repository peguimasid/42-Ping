#include "../includes/ft-ping.h"

int main(int argc, char **argv) {
  (void)argv;
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc != 2) {
    fatal_error("Usage: ft_ping [-v verbose] [-h help] hostname");
  }

  t_context *ctx = init_context();
  free(ctx);

  return EXIT_SUCCESS;
}
