#include "../includes/ft-ping.h"

t_context *ctx;

void handle_arg(char *arg) {
  if (strcmp(arg, "-h") == 0) {
    printf(USAGE);
    free(ctx);
    return exit(EXIT_SUCCESS);
  }

  if (strcmp(arg, "-v") == 0) {
    ctx->verbose_mode = true;
    return;
  }

  if (arg[0] == '-') {
    fatal_error(USAGE);
    free(ctx);
    return;
  }

  ctx->hostname = arg;
}

void parse_args(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    handle_arg(argv[i]);
  }
}

int main(int argc, char **argv) {
  if (getuid() != 0) {
    fatal_error("ft_ping: Operation not permitted (you must be root)\n");
  }

  if (argc < 2) {
    fatal_error(USAGE);
  }

  init_context();
  parse_args(argc, argv);
  free(ctx);

  return EXIT_SUCCESS;
}
