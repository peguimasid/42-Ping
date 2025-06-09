#include "../includes/ft-ping.h"

t_context *ctx;

void handle_arg(char *arg) {
  if (strcmp(arg, "-h") == 0) {
    printf(USAGE);
    free(ctx);
    exit(EXIT_SUCCESS);
  }

  if (strcmp(arg, "-v") == 0) {
    ctx->verbose_mode = true;
    return;
  }

  if (arg[0] == '-') {
    fatal_error(USAGE);
    return;
  }

  ctx->target = arg;

  set_address_by_target(ctx->target);
}

void parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    handle_arg(argv[i]);
  }

  if (!ctx->target_addr) {
    fatal_error("ft_ping: usage error: Destination address required\n");
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
  set_signal_handlers();
  free(ctx);

  return EXIT_SUCCESS;
}
