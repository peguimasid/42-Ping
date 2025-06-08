#include "../includes/ft-ping.h"

t_context *ctx;

void parse_args(int argc, char **argv) {
  for (int i = i; i < argc; i++) {
    printf("arg = %s\n", argv[i]);
  }
}

int main(int argc, char **argv) {
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc < 2) {
    fatal_error("Usage: ft_ping [-v verbose] [-h help] hostname");
  }

  init_context();
  parse_args(argc, argv);
  free(ctx);

  return EXIT_SUCCESS;
}
